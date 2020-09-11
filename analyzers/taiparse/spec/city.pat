###############################################
# FILE: city.pat
# SUBJ: Automatically generated.
# AUTH: file2pass
# CREATED: Thu Jan 13 11:37:10 2005
###############################################

 @PATH _ROOT _TEXTZONE

@POST
  if (N(5))
    L("last") = N(5);
  else if (N(3))
    L("last") = N(3);
  else
    L("last") = N(1);
  setposrange(N(1),L("last"),"NP");

  S("ne text") = phrasetext();
  S("ne") = 1;
  S("sem") = S("ne type") = "location";
  S("ne type conf") = 95;
  S("sem location") = "city";
  S("city") = 1;
  S("mypos") = "NP";
  S("stem") = phrasetext();
  S("number") = "singular";
  if (G("verbose"))
    "city.txt" << phrasetext() << "\n";
  single();
@RULES

_city [layer=(_noun)] <- 
	Agartala
	@@

_city [layer=(_noun)] <- 
	Ahmadabad
	@@

_city [layer=(_noun)] <- 
	Aizawl
	@@

_city [layer=(_noun)] <- 
	Alor
	_xWHITE [star]
	Setar
	@@

_city [layer=(_noun)] <- 
	Ang
	_xWHITE [star]
	Thong
	@@

_city [layer=(_noun)] <- 
	Anuradhapura
	@@

_city [layer=(_noun)] <- 
	Attapu
	@@

_city [layer=(_noun)] <- 
	Bac
	_xWHITE [star]
	Lieu
	@@

_city [layer=(_noun)] <- 
	Badulla
	@@

_city [layer=(_noun)] <- 
	Balikpapan
	@@

_city [layer=(_noun)] <- 
	Banda
	_xWHITE [star]
	Aceh
	@@

_city [layer=(_noun)] <- 
	Bandar
	_xWHITE [star]
	Seri
	_xWHITE [star]
	Begawan
	@@

_city [layer=(_noun)] <- 
	Bandjermasin
	@@

_city [layer=(_noun)] <- 
	Bangalore
	@@

_city [layer=(_noun)] <- 
	Bangkok
	@@
_city [layer=(_noun)] <- 
	Jakarta
	@@

_city [layer=(_noun)] <- 
	Barisal
	@@

_city [layer=(_noun)] <- 
	Bassein
	@@

_city [layer=(_noun)] <- 
	Batdambang
	@@

_city [layer=(_noun)] <- 
	Bengkulu
	@@

_city [layer=(_noun)] <- 
	Bhopal
	@@

_city [layer=(_noun)] <- 
	Bhubaneshwar
	@@

_city [layer=(_noun)] <- 
	Bien
	_xWHITE [star]
	Hoa
	@@

_city [layer=(_noun)] <- 
	Bombay
	@@

_city [layer=(_noun)] <- 
	Buon
	_xWHITE [star]
	Me
	_xWHITE [star]
	Thuot
	@@

_city [layer=(_noun)] <- 
	Buriram
	@@

_city [layer=(_noun)] <- 
	Calcutta
	@@

_city [layer=(_noun)] <- 
	Can
	_xWHITE [star]
	Tho
	@@

_city [layer=(_noun)] <- 
	Cao
	_xWHITE [star]
	Bang
	@@

_city [layer=(_noun)] <- 
	Cao
	_xWHITE [star]
	Lanh
	@@

_city [layer=(_noun)] <- 
	Chachoengsao
	@@

_city [layer=(_noun)] <- 
	Chainat
	@@

_city [layer=(_noun)] <- 
	Chaiyaphum
	@@

_city [layer=(_noun)] <- 
	Champasak
	@@

_city [layer=(_noun)] <- 
	Chang
	_xWHITE [star]
	Rai
	@@

_city [layer=(_noun)] <- 
	Chanthaburi
	@@

_city [layer=(_noun)] <- 
	Chiang
	_xWHITE [star]
	Mai
	@@

_city [layer=(_noun)] <- 
	Chittagong
	@@

_city [layer=(_noun)] <- 
	Chon
	_xWHITE [star]
	Buri
	@@

_city [layer=(_noun)] <- 
	Chumphon
	@@

_city [layer=(_noun)] <- 
	Cochin
	@@

_city [layer=(_noun)] <- 
	Colombo
	@@

_city [layer=(_noun)] <- 
	Da
	_xWHITE [star]
	Lat
	@@

_city [layer=(_noun)] <- 
	Da
	_xWHITE [star]
	Nang
	@@

_city [layer=(_noun)] <- 
	Dacca
	@@

_city [layer=(_noun)] <- 
	Daman
	@@

_city [layer=(_noun)] <- 
	Dong
	_xWHITE [star]
	Ha
	@@

_city [layer=(_noun)] <- 
	Dong
	_xWHITE [star]
	Hoi
	@@

_city [layer=(_noun)] <- 
	Fangcheng
	_xWHITE [star]
	Gang
	@@

_city [layer=(_noun)] <- 
	Galle
	@@

_city [layer=(_noun)] <- 
	Gandhinagar
	@@

_city [layer=(_noun)] <- 
	Guangzhou
	@@

_city [layer=(_noun)] <- 
	Ha
	_xWHITE [star]
	Giang
	@@

_city [layer=(_noun)] <- 
	Ha
	_xWHITE [star]
	Tinh
	@@

_city [layer=(_noun)] <- 
	Hai
	_xWHITE [star]
	Duong
	@@

_city [layer=(_noun)] <- 
	Haiphong
	@@

_city [layer=(_noun)] <- 
	Haka
	@@

_city [layer=(_noun)] <- 
	Hanoi
	@@

_city [layer=(_noun)] <- 
	Ho
	_xWHITE [star]
	Chi
	_xWHITE [star]
	Minh
	_xWHITE [star]
	City
	@@

_city [layer=(_noun)] <- 
	Hoa
	_xWHITE [star]
	Binh
	@@

_city [layer=(_noun)] <- 
	Hon
	_xWHITE [star]
	Gai
	@@

_city [layer=(_noun)] <- 
	Hue
	@@

_city [layer=(_noun)] <- 
	Hyderabad
	@@

_city [layer=(_noun)] <- 
	Imphal
	@@

_city [layer=(_noun)] <- 
	Ipoh
	@@

_city [layer=(_noun)] <- 
	Jambi
	@@

_city [layer=(_noun)] <- 
	Johor
	_xWHITE [star]
	Baharu
	@@

_city [layer=(_noun)] <- 
	Kalasin
	@@

_city [layer=(_noun)] <- 
	Kamphaeng
	_xWHITE [star]
	Phet
	@@

_city [layer=(_noun)] <- 
	Kampong
	_xWHITE [star]
	Cham
	@@

_city [layer=(_noun)] <- 
	Kampong
	_xWHITE [star]
	Chnang
	@@

_city [layer=(_noun)] <- 
	Kampong
	_xWHITE [star]
	Spoe
	@@

_city [layer=(_noun)] <- 
	Kampong
	_xWHITE [star]
	Thum
	@@

_city [layer=(_noun)] <- 
	Kampot
	@@

_city [layer=(_noun)] <- 
	Kanchanaburi
	@@

_city [layer=(_noun)] <- 
	Kandy
	@@

_city [layer=(_noun)] <- 
	Kangar
	@@

_city [layer=(_noun)] <- 
	Kavaratti
	@@

_city [layer=(_noun)] <- 
	Kemaman
	_xWHITE [star]
	Harbor
	@@

_city [layer=(_noun)] <- 
	Khon
	_xWHITE [star]
	Kaen
	@@

_city [layer=(_noun)] <- 
	Khulna
	@@

_city [layer=(_noun)] <- 
	Kon
	_xWHITE [star]
	Tum
	@@

_city [layer=(_noun)] <- 
	Kota
	_xWHITE [star]
	Baharu
	@@

_city [layer=(_noun)] <- 
	Kota
	_xWHITE [star]
	Kinabalu
	@@

_city [layer=(_noun)] <- 
	Kowloon
	@@

_city [layer=(_noun)] <- 
	Krabi
	@@

_city [layer=(_noun)] <- 
	Kracheh
	@@

_city [layer=(_noun)] <- 
	Krong
	_xWHITE [star]
	Kaoh
	_xWHITE [star]
	Kong
	@@
_city [layer=(_noun)] <- 
	Hong
	_xWHITE [star]
	Kong
	@@

_city [layer=(_noun)] <- 
	Kuala
	_xWHITE [star]
	Lipis
	@@

_city [layer=(_noun)] <- 
	Kuala
	_xWHITE [star]
	Lumpur
	@@

_city [layer=(_noun)] <- 
	Kuala
	_xWHITE [star]
	Terengganu
	@@

_city [layer=(_noun)] <- 
	Kuantan
	_xWHITE [star]
	New
	_xWHITE [star]
	Port
	@@

_city [layer=(_noun)] <- 
	Kuching
	@@

_city [layer=(_noun)] <- 
	Lampang
	@@

_city [layer=(_noun)] <- 
	Lamphun
	@@

_city [layer=(_noun)] <- 
	Lang
	_xWHITE [star]
	Son
	@@

_city [layer=(_noun)] <- 
	Lao
	_xWHITE [star]
	Cai
	@@

_city [layer=(_noun)] <- 
	Loei
	@@

_city [layer=(_noun)] <- 
	Loikaw
	@@

_city [layer=(_noun)] <- 
	Long
	_xWHITE [star]
	Xuyen
	@@

_city [layer=(_noun)] <- 
	Louang
	_xWHITE [star]
	Namtha
	@@

_city [layer=(_noun)] <- 
	Louangphrabang
	@@

_city [layer=(_noun)] <- 
	Luan
	_xWHITE [star]
	Chau
	@@

_city [layer=(_noun)] <- 
	Lumphat
	@@

_city [layer=(_noun)] <- 
	Macau
	@@

_city [layer=(_noun)] <- 
	Madras
	@@

_city [layer=(_noun)] <- 
	Madurai
	@@

_city [layer=(_noun)] <- 
	Mae
	_xWHITE [star]
	Hong
	_xWHITE [star]
	Son
	@@

_city [layer=(_noun)] <- 
	Magway
	@@

_city [layer=(_noun)] <- 
	Maha
	_xWHITE [star]
	Sarakham
	@@

_city [layer=(_noun)] <- 
	Mandalay
	@@

_city [layer=(_noun)] <- 
	Mangalore
	@@

_city [layer=(_noun)] <- 
	Medan
	@@

_city [layer=(_noun)] <- 
	Melaka
	@@

_city [layer=(_noun)] <- 
	Moulmein
	@@

_city [layer=(_noun)] <- 
	Muang
	_xWHITE [star]
	Khammouan
	@@

_city [layer=(_noun)] <- 
	Muang
	_xWHITE [star]
	Xaignabouri
	@@

_city [layer=(_noun)] <- 
	My
	_xWHITE [star]
	Tho
	@@

_city [layer=(_noun)] <- 
	Nagpur
	@@

_city [layer=(_noun)] <- 
	Nakhom
	_xWHITE [star]
	Phanom
	@@

_city [layer=(_noun)] <- 
	Nakhon
	_xWHITE [star]
	Nayok
	@@

_city [layer=(_noun)] <- 
	Nakhon
	_xWHITE [star]
	Pathom
	@@

_city [layer=(_noun)] <- 
	Nakhon
	_xWHITE [star]
	Ratchasima
	@@

_city [layer=(_noun)] <- 
	Nakhon
	_xWHITE [star]
	Sawan
	@@

_city [layer=(_noun)] <- 
	Nakhon
	_xWHITE [star]
	Si
	_xWHITE [star]
	Thammarat
	@@

_city [layer=(_noun)] <- 
	Nan
	@@

_city [layer=(_noun)] <- 
	Nanning
	@@

_city [layer=(_noun)] <- 
	Narathiwat
	@@

# So it won't be tagged as plural.
_city [layer=(_noun)] <- 
	New
	_xWHITE [star]
	Orleans
	@@

_city [layer=(_noun)] <- 
	Nha
	_xWHITE [star]
	Trang
	@@

_city [layer=(_noun)] <- 
	Ninh
	_xWHITE [star]
	Binh
	@@

_city [layer=(_noun)] <- 
	Nong
	_xWHITE [star]
	Khai
	@@

_city [layer=(_noun)] <- 
	Nonthaburi
	@@

_city [layer=(_noun)] <- 
	Pa
	_xWHITE [star]
	\-
	_xWHITE [star]
	an
	@@

_city [layer=(_noun)] <- 
	Padang
	@@

_city [layer=(_noun)] <- 
	Palangkaraya
	@@

_city [layer=(_noun)] <- 
	Palembang
	@@

_city [layer=(_noun)] <- 
	Panaji
	@@

_city [layer=(_noun)] <- 
	Pathum
	_xWHITE [star]
	Thani
	@@

_city [layer=(_noun)] <- 
	Pattani
	@@

_city [layer=(_noun)] <- 
	Pegu
	@@

_city [layer=(_noun)] <- 
	Pekanbaru
	@@

_city [layer=(_noun)] <- 
	Phan
	_xWHITE [star]
	Thiet
	@@

_city [layer=(_noun)] <- 
	Phangnga
	@@

_city [layer=(_noun)] <- 
	Phatthalung
	@@

_city [layer=(_noun)] <- 
	Phayao
	@@

_city [layer=(_noun)] <- 
	Phet
	_xWHITE [star]
	Buri
	@@

_city [layer=(_noun)] <- 
	Phetchabun
	@@

_city [layer=(_noun)] <- 
	Phichit
	@@

_city [layer=(_noun)] <- 
	Phitsanulok
	@@

_city [layer=(_noun)] <- 
	Phnom
	_xWHITE [star]
	Penh
	@@

_city [layer=(_noun)] <- 
	Phnum
	_xWHITE [star]
	Tbeng
	_xWHITE [star]
	Meanchey
	@@

_city [layer=(_noun)] <- 
	Phongsali
	@@

_city [layer=(_noun)] <- 
	Phra
	_xWHITE [star]
	Nakhon
	_xWHITE [star]
	Si
	_xWHITE [star]
	Ayutthaya
	@@

_city [layer=(_noun)] <- 
	Phrae
	@@

_city [layer=(_noun)] <- 
	Phu
	_xWHITE [star]
	Lang
	_xWHITE [star]
	Thuong
	@@

_city [layer=(_noun)] <- 
	Phuket
	@@

_city [layer=(_noun)] <- 
	Melbourne
	@@

_city [layer=(_noun)] <- 
	Pinang
	@@

_city [layer=(_noun)] <- 
	Play
	_xWHITE [star]
	Cu
	@@

_city [layer=(_noun)] <- 
	Pondicherry
	@@

_city [layer=(_noun)] <- 
	Pontianak
	@@

_city [layer=(_noun)] <- 
	Port
	_xWHITE [star]
	Blair
	@@

_city [layer=(_noun)] <- 
	Pouthisat
	@@

_city [layer=(_noun)] <- 
	Prachin
	_xWHITE [star]
	Buri
	@@

_city [layer=(_noun)] <- 
	Prachuap
	_xWHITE [star]
	Khiri
	_xWHITE [star]
	Khan
	@@

_city [layer=(_noun)] <- 
	Prey
	_xWHITE [star]
	Veng
	@@

_city [layer=(_noun)] <- 
	Pune
	@@

_city [layer=(_noun)] <- 
	Puttalan
	@@

_city [layer=(_noun)] <- 
	Quang
	_xWHITE [star]
	Ngai
	@@

_city [layer=(_noun)] <- 
	Qui
	_xWHITE [star]
	Nhon
	@@

_city [layer=(_noun)] <- 
	Rach
	_xWHITE [star]
	Gia
	@@

_city [layer=(_noun)] <- 
	Rajshahi
	@@

_city [layer=(_noun)] <- 
	Rangoon
	@@

_city [layer=(_noun)] <- 
	Ranong
	@@

_city [layer=(_noun)] <- 
	Ratchaburi
	@@

_city [layer=(_noun)] <- 
	Ratnapura
	@@

_city [layer=(_noun)] <- 
	Rayong
	@@

_city [layer=(_noun)] <- 
	Roi
	_xWHITE [star]
	Et
	@@

_city [layer=(_noun)] <- 
	Sagaing
	@@

_city [layer=(_noun)] <- 
	Sakon
	_xWHITE [star]
	Nakhon
	@@

_city [layer=(_noun)] <- 
	Samarinda
	@@

_city [layer=(_noun)] <- 
	Samut
	_xWHITE [star]
	Prakan
	@@

_city [layer=(_noun)] <- 
	Samut
	_xWHITE [star]
	Sakhon
	@@

_city [layer=(_noun)] <- 
	Samut
	_xWHITE [star]
	Songkhram
	@@

_city [layer=(_noun)] <- 
	Saraburi
	@@

_city [layer=(_noun)] <- 
	Saravan
	@@

_city [layer=(_noun)] <- 
	Satun
	@@

_city [layer=(_noun)] <- 
	Savannakhet
	@@

_city [layer=(_noun)] <- 
	Senmonorom
	@@

_city [layer=(_noun)] <- 
	Seremban
	@@

_city [layer=(_noun)] <- 
	Shah
	_xWHITE [star]
	Alam
	@@

_city [layer=(_noun)] <- 
	Siemreab
	@@

_city [layer=(_noun)] <- 
	Silvassa
	@@

_city [layer=(_noun)] <- 
	Sing
	_xWHITE [star]
	Buri
	@@

_city [layer=(_noun)] <- 
	Singapore
	@@

_city [layer=(_noun)] <- 
	Sisaket
	@@

_city [layer=(_noun)] <- 
	Sittwe
	@@

_city [layer=(_noun)] <- 
	Soc
	_xWHITE [star]
	Trang
	@@

_city [layer=(_noun)] <- 
	Son
	_xWHITE [star]
	La
	@@

_city [layer=(_noun)] <- 
	Songkhla
	@@

_city [layer=(_noun)] <- 
	Stoeng
	_xWHITE [star]
	Treng
	@@

_city [layer=(_noun)] <- 
	Sukhothai
	@@

_city [layer=(_noun)] <- 
	Supham
	_xWHITE [star]
	Buri
	@@

_city [layer=(_noun)] <- 
	Surat
	_xWHITE [star]
	Thani
	@@

_city [layer=(_noun)] <- 
	Surin
	@@

_city [layer=(_noun)] <- 
	Svay
	_xWHITE [star]
	Rieng
	@@

_city [layer=(_noun)] <- 
	Tak
	@@

_city [layer=(_noun)] <- 
	Takev
	@@

_city [layer=(_noun)] <- 
	Tan
	_xWHITE [star]
	An
	@@

_city [layer=(_noun)] <- 
	Taunggyi
	@@

_city [layer=(_noun)] <- 
	Tavoy
	@@

_city [layer=(_noun)] <- 
	Tay
	_xWHITE [star]
	Ninh
	@@

_city [layer=(_noun)] <- 
	Thahanbok
	_xWHITE [star]
	Lop
	_xWHITE [star]
	Buri
	@@

_city [layer=(_noun)] <- 
	Thai
	_xWHITE [star]
	Binh
	@@

_city [layer=(_noun)] <- 
	Thai
	_xWHITE [star]
	Nguyen
	@@

_city [layer=(_noun)] <- 
	Thanh
	_xWHITE [star]
	Hoa
	@@

_city [layer=(_noun)] <- 
	Thu
	_xWHITE [star]
	Dau
	_xWHITE [star]
	Mot
	@@

_city [layer=(_noun)] <- 
	Tra
	_xWHITE [star]
	Vinh
	@@

_city [layer=(_noun)] <- 
	Trang
	@@

_city [layer=(_noun)] <- 
	Trat
	@@

_city [layer=(_noun)] <- 
	Trincomalee
	@@

_city [layer=(_noun)] <- 
	Trivandrum
	@@

_city [layer=(_noun)] <- 
	Truc
	_xWHITE [star]
	Giang
	@@

_city [layer=(_noun)] <- 
	Tuy
	_xWHITE [star]
	Hoa
	@@

_city [layer=(_noun)] <- 
	Tuyen
	_xWHITE [star]
	Quang
	@@

_city [layer=(_noun)] <- 
	Ubon
	_xWHITE [star]
	Ratchathani
	@@

_city [layer=(_noun)] <- 
	Udon
	_xWHITE [star]
	Thani
	@@

_city [layer=(_noun)] <- 
	Uthai
	_xWHITE [star]
	Thani
	@@

_city [layer=(_noun)] <- 
	Uttaradit
	@@

_city [layer=(_noun)] <- 
	Victoria
	@@

_city [layer=(_noun)] <- 
	Vientiane
	@@

_city [layer=(_noun)] <- 
	Viet
	_xWHITE [star]
	Tri
	@@

_city [layer=(_noun)] <- 
	Vinh
	@@

_city [layer=(_noun)] <- 
	Vinh
	_xWHITE [star]
	Long
	@@

_city [layer=(_noun)] <- 
	Vishakhapatnam
	@@

_city [layer=(_noun)] <- 
	Xam
	_xWHITE [star]
	Nua
	@@

_city [layer=(_noun)] <- 
	Xiangkhoang
	@@

_city [layer=(_noun)] <- 
	Yala
	@@

_city [layer=(_noun)] <- 
	Yasothon
	@@

_city [layer=(_noun)] <- 
	Yen
	_xWHITE [star]
	Bai
	@@

