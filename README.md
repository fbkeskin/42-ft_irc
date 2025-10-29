<div align="center">
	<h1>💬 Internet Relay Chat, ft_irc </h1>
  "This project is about creating your own IRC server.
You will use an actual IRC client to connect to your server and test it.
The Internet is governed by solid standard protocols that allow connected computers to
interact with each other.
It’s always beneficial to understand these protocols."
<br> <br>  
	


https://github.com/user-attachments/assets/a4d569b8-7e75-4b5c-bb57-5d2844637aef


  
  <p align="center">
	<h4>My first own IRC Server!<br>
    <br>
  </p></h4>
    	<img src="https://img.shields.io/badge/norminette-not necessary-success"/>
	<a href="https://developer.apple.com/library/archive/documentation/Performance/Conceptual/ManagingMemory/Articles/FindingLeaks.html"><img src="https://img.shields.io/badge/leaks-none-success" /></a>
	<img src="https://img.shields.io/badge/-100%2F125-success?logo=42&logoColor=fff" />
  <p align="center">
    <br>
    <i>Internet Relay Chat or IRC is a text-based communication protocol on the Internet.
It offers real-time messaging that can be either public or private. Users can exchange
direct messages and join group channels.
IRC clients connect to IRC servers in order to join channels. IRC servers are connected
together to form a network.<br><br>
</i>
  </p>
  <br />

</div>

## 📝Usage
**1.** Repo'yu clone'layın: 

```bash
git clone https://github.com/fbkeskin/42-ft_irc.git
cd ft_irc
```

**2.** Makefile'ı kullanarak source kodu derleyin:
> **ft_irc**, köklü bir internet protokolü olan **Internet Relay Chat (IRC)** için **C++98 standardına** uygun olarak geliştirilmiş, tam özellikli, multiple clients bir server'dır. Bu proje, kullanıcıların genel kanallara katılabileceği ve private mesajlaşmada bulunabileceği real-time, text based iletişimi mümkün kılmayı amaçlamaktadır. C++98 kısıtlamaları dahilinde **network programming**, **concurrent client management** ve **protocol implementation** yetkinliklerini derinleştirmeye odaklanmıştır.
```bash
make
```
> 1 executable file oluşmaktadır: `ircserv`.

**3.** Server, TCP protokolünü destlekler. Cihazınızda uygun bir TCP portu ve server'a bağlantı için gerekli bir password belirleyin. Ardından, `ircserv` binary dosyasını `./ircserv <port> <password>` formatında execute ederek programı başlatın:
> IRC server, genellikle 6667 portu üzerinden hizmet verir.
```bash
$>./ircserv 6667 efrasiyab1010
server listening, fd: 3

$>./ircserv 678 12345
Error: Invalid Port Range!

$>./ircserv 6666 "pass word"
Error: Invalid Password Format!

$>./ircserv 7777 12345 ahriman
Error: Invalid Argument Input!
```

> Program, OS'den bir soket tahsis ederek listening işlemine başlar. Bir client connection'ınını kabul etmeye ve bağlı client'tan gelen data request, response işlemlerine hazırdır.

**4.** 2 farklı şekilde server'a bağlanabilirsiniz.
  İlk olarak terminal üzerinden netcat ile;
  ```bash
  $>nc -C localhost 6667
  ```
> netcat bağlantılarında IRC server'ın uygun mesaj formatını desteklemesi için(çoğu web server'larda olduğu gibi mesaj sonu \r\n ile biter) `-C` flag'i eklemelisiniz. Aksi takdirde server'a attığınız request'ler format hatası verip handle edilmeyecektir.

  Diğer bağlanma yöntemi hazır bir client kullanmaktır. Bu projede HexChat client olarak seçilmiş ve ona uygun konfigürasyonlar yapılmıştır. kvirc, weechat gibi alternatifler de bulunmaktadır.
  HexChat indirmek için;
  ```bash
  $>pkgtool install HexChat
  ```
  **5.** Ardından HexChat arayüzünde gerekli ayarları gerçekleştirip connect olun.
  
  <img width="300" height="400" alt="Screenshot from 2025-10-29 14-50-11" src="https://github.com/user-attachments/assets/4bf2f0da-865b-47a0-8a0a-dd026e0a01bd" />
    <br>
  
**6.** Bağlantı ardından PASS, NICk ve USER komutu ile server'a register olun.

<img width="500" height="400" alt="Screenshot from 2025-10-29 18-17-15" src="https://github.com/user-attachments/assets/f40802a3-c5a7-44d9-8ca7-ae92c046e97d" />
<img width="500" height="400" alt="Screenshot from 2025-10-29 18-28-23" src="https://github.com/user-attachments/assets/5469eade-f62f-4b75-b272-5ce130bf16bc" />

---

## 👤 Kullanıcı ve Temel İletişim

* **Bağlantı ve Kimlik Doğrulama:** Gerekli bir parola ile bağlantı kurma (`PASS`), takma ad (`NICK`) ve kullanıcı adı (`USER`) belirleme.
* **Kanal Katılımı:** Kullanıcıların kanallara katılmasına izin verme (`JOIN`).
* **Mesajlaşma:** Hem kanallara hem de özel kullanıcılara mesaj gönderme ve alma (`PRIVMSG`).
* **Eş Zamanlı Client Yönetimi:** Server, **engelleyici olmayan (non-blocking) G/Ç işlemleri** kullanarak **aynı anda birden fazla istemciyi** yönetme kapasitesine sahiptir. Bu, sistem kaynaklarını verimli kullanmak için tek bir `poll()` çağrısı kullanılarak tüm okuma, yazma ve dinleme işlemlerinin yönetilmesiyle sağlanır.

---

## 👑 Operatör Komutları ve Kanal Yönetimi

Kanal operatörleri, kanallarını yönetmek ve düzenlemek için özel komut yetkilerine sahiptir:

* **`KICK`**: Bir client'ı kanaldan atma.
* **`INVITE`**: Bir client'ı kanala davet etme.
* **`TOPIC`**: Kanalın konusunu görüntüleme veya değiştirme.
* **`MODE`**: Kanal ayarlarını değiştirme:
    * **`i`**: **Sadece davetle girilebilen** kanal (Invite-only).
    * **`t`**: Konu değiştirme yetkisini yalnızca kanal operatörleriyle kısıtlama.
    * **`k`**: Kanal **anahtarı (parolası)** belirleme/kaldırma.
    * **`o`**: Kullanıcılara kanal **operatör yetkisi** verme/alma.
    * **`l`**: Kanaldaki **kullanıcı sayısını sınırlandırma** (user limit).




