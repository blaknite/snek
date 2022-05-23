# snek

Snake for the RC2014 Pro CP/M.

![](https://d1bh5m8o3ysx9y.cloudfront.net/uploads/images/8f3de0a2-5199-48a8-8d71-18139fba0d0b.png)

![](https://d1bh5m8o3ysx9y.cloudfront.net/uploads/images/151a6760-73a0-4924-91bf-92fbb90de8ca.png)

## Give it a go

1. Download latest release of SNEK.COM
1. Use the RC2014 to CPM HEX file conversion tool transfer it to your RC2014
1. Enjoy!

## Compile it yourself

Snek is written for the z88dk RC2014 subtype. Compiling it yourself is done with one line.

```
zcc +rc2014 -subtype=cpm -SO3 -clib=sdcc_iy snek.c snek.asm -o snek -create-app
```
