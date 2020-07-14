# UDP DOSYA TRANSFER SİSTEMİ

## KULLANIMI

### SUNUCU

sunucu dosyasının içinde açılan terminale make yazmak yeterlidir

```
make
```

Kod compile olduktan sonra, argüman olarak kullanılmak istenen port numarası verilmelidir.

```
./sunucu 3250 (3250 rastgele verilmiş port numarasıdır.)
```

## İSTEMCI

istemci dosyasının içinde açılan terminale make yazmak yeterlidir

```
make
```

Kod compile olduktan sonra, argüman olarak kullanılmak istenen port numarası verilmelidir.

```
./istemci 127.0.0.1 3250
```

ilk argüman olarak ip adresi (127.0.0.1) verilmelidir, 2. argüman olarak ise sunucu da açılan port yazılmalıdır.

Dosya için demo.mp4 dosyası ile görüntülü bir demo sunulmuştur.
