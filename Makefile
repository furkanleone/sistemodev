# Sistem Programlama Ödevi - Makefile
# Kullanim: make        -> her ikisini derle
#           make soru1  -> yalnizca soru1
#           make soru2  -> yalnizca soru2
#           make clean  -> derlenmiş dosyaları sil
#           make run1   -> soru1'i calistir
#           make run2   -> soru2'yi calistir

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11

TARGETS = soru1_matris soru2_sinyal

.PHONY: all soru1 soru2 clean run1 run2

all: $(TARGETS)
	@echo "Her iki program basariyla derlendi."

soru1: soru1_matris

soru2: soru2_sinyal

soru1_matris: soru1_matris.c
	$(CC) $(CFLAGS) -o $@ $<
	@echo "soru1_matris derlendi."

soru2_sinyal: soru2_sinyal.c
	$(CC) $(CFLAGS) -o $@ $<
	@echo "soru2_sinyal derlendi."

run1: soru1_matris
	./soru1_matris

run2: soru2_sinyal
	./soru2_sinyal

clean:
	rm -f $(TARGETS)
	@echo "Temizlendi."
