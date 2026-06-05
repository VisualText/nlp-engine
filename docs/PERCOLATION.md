# Cross-repo release percolation

How a change in one VisualText repo automatically flows downstream to the repos
that embed it — no nightly polling, no manual dispatch. This is the single
source of truth for the wiring; the individual workflow files link back here.

## The chain

```
parse-en-us ──(v* tag)──► parse-en-us-release ──► analyzers
                                              ├──► analyzer-templates
                                              └──► visualtext-files

analyzer-templates ──(v* tag)──► analyzer-templates-release ──► nlp-engine-linux
                                                            ├──► nlp-engine-windows
                                                            ├──► nlp-engine-mac
                                                            └──► visualtext-files

analyzers ──(after its auto-bump release)──► analyzers-release ──► nlp-engine
                                                                   (opens a PR)

nlp-engine ──(release / move-assets)──► nlp-engine-release ──► nlp-engine-linux
                                                           ├──► nlp-engine-windows
                                                           └──► nlp-engine-mac
```

## Mechanism

GitHub's `repository_dispatch`: a **sender** workflow calls
`peter-evans/repository-dispatch` to fire a named event at a **listener** repo,
which has a workflow triggered by `repository_dispatch: types: [<event>]`.

- **Trigger:** senders fire on a release tag push (`push: tags: ['v*']`), except
  `analyzers`, which pings `nlp-engine` from inside its own bump job after it
  releases. All listeners also keep a manual `workflow_dispatch` button.
- **Secret:** the dispatch call needs a real credential — the default
  `GITHUB_TOKEN` **cannot trigger workflows in other repos** by design. We use a
  classic PAT (`repo` scope) stored as the secret **`CLASSIC_PAT`** in every
  **sender** repo. Listener-only repos don't need it.

## Who sends what

| Sender | Fires on | event-type | Listeners | Sender workflow |
|---|---|---|---|---|
| parse-en-us | `v*` tag | `parse-en-us-release` | analyzers, analyzer-templates, visualtext-files | `dispatch-update-parse-en-us.yml` |
| analyzer-templates | `v*` tag | `analyzer-templates-release` | nlp-engine-{linux,windows,mac}, visualtext-files | `dispatch-update-analyzer-templates.yml` |
| analyzers | end of its bump job | `analyzers-release` | nlp-engine | `parse-en-us.yml` (final step) |
| nlp-engine | release / build complete | `nlp-engine-release` | nlp-engine-{linux,windows,mac} | `move-assets.yml` |

## Who listens for what

| Listener | event-type | Listener workflow | What it does |
|---|---|---|---|
| analyzers | `parse-en-us-release` | `parse-en-us.yml` | bump parse-en-us submodule, version-tag, release, then ping nlp-engine |
| analyzer-templates | `parse-en-us-release` | `update-parse-en-us.yml` | bump parse-en-us submodule, version-tag (pushed with `CLASSIC_PAT` so the tag fires its own `dispatch-update-analyzer-templates.yml`) |
| visualtext-files | `parse-en-us-release` | `update-parse-en-us.yml` | bump parse-en-us submodule, version-tag, release |
| visualtext-files | `analyzer-templates-release` | `update-analyzer-templates.yml` | bump analyzer-templates submodule |
| nlp-engine | `analyzers-release` | `update-analyzers.yml` | open a PR bumping the analyzers submodule (no auto-release) |
| nlp-engine-{linux,windows,mac} | `analyzer-templates-release` | `update-analyzer-templates.yml` | bump analyzer-templates submodule |
| nlp-engine-{linux,windows,mac} | `nlp-engine-release` | `nlp-engine-build.yml` | build the platform package |

## Submodule embedding (what bumps what)

- **analyzers** embeds `parse-en-us`, `nlp-tutorials`, `nlpfix-analyzers`.
- **analyzer-templates** embeds `parse-en-us` (at path `parse-en-us`).
- **visualtext-files** embeds `parse-en-us` (at `analyzers/parse-en-us`) and `analyzer-templates`.
- **nlp-engine** embeds `analyzers` and `vcpkg`.
  - ⚠️ **Never auto-bump `vcpkg`** — it is intentionally pinned. Only the
    `analyzers` submodule is bumped automatically (`git submodule update
    --remote analyzers`, not bare `--remote`).

## Why two different downstream behaviors

- **analyzers / visualtext-files** auto-bump, version-tag and cut a release. The
  release asset (`analyzers.zip` / `visualtext.zip`) is built and attached *in
  the same job*, because a release created with `GITHUB_TOKEN` does not fire the
  `release: created` event, so a separate "attach assets" workflow would never
  run for an auto-release.
- **nlp-engine** opens a **PR** instead of auto-releasing: it has its own version
  (`NLP_ENGINE_VERSION` in `nlp/main.cpp`) and a heavy per-platform release
  pipeline, so a submodule bump shouldn't auto-cut an engine release or land on
  `master` unreviewed. Merging the PR (a human action) triggers the normal
  build/test jobs.

## Testing the chain without a real release

Run the relevant sender by hand: **Actions → "Dispatch update-…" → Run
workflow** (its `workflow_dispatch` button), or push a `v*` tag. You can also run
any listener directly from its own Actions tab. Each listener no-ops cleanly if
its submodule pointer is already current.

## Adding a new link

1. In the upstream (sender) repo, add a `dispatch-*.yml` that fires on `v*` and
   dispatches `<repo>-release` to the embedding repos (matrix fan-out, `token:
   ${{ secrets.CLASSIC_PAT }}`, `continue-on-error: true`).
2. Add `CLASSIC_PAT` (same classic PAT) as a repo secret in that sender.
3. In each downstream (listener) repo, add `repository_dispatch: types:
   [<repo>-release]` to the workflow that updates that submodule, defaulting any
   version bump to `patch` on a ping (a `repository_dispatch` event has no
   `inputs.bump`).

## Gotcha: re-attaching a submodule that workflows fetch manually

`nlp-engine` build/test jobs historically `git clone`d `analyzers` fresh into
`./analyzers`. When the `analyzers` gitlink was re-attached, a `submodules: true`
checkout started populating `./analyzers`, colliding with that fetch (`git clone`
refuses a non-empty destination). The fix: `rm -rf analyzers` before the fresh
fetch in `build-linux.yml` / `build-macos.yml`, and add `submodules: recursive`
to `build-visualfiles.yml` so the packaged `visualtext.zip` actually contains
`analyzers/`.
