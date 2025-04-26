/**
*
* @author Aybuke Gokce Yavas aybuke.yavas@ogr.sakarya.edu.tr
* @since 20/04/2025
* <p>
* Tarih ve saat hesabi ve takibi
* </p>
*/


package uzaysim;

public class zaman 
{
	private int gun;
	private int ay;
	private int yil;
	private int saat;
	
	
	public zaman kopyala() {
	    zaman yeniZaman = new zaman(this.gun, this.ay, this.yil);
	    yeniZaman.setSaat(this.saat);
	    return yeniZaman;
	}
	
	
	public zaman(int gun, int ay, int yil) 
	{
		this.gun = gun;
		this.ay = ay;
		this.yil = yil;
		this.saat = 0;
	}
	
	
	public void saatleriIlerle(int kacSaat, int gunSaatSayisi) {
	    for (int i = 0; i < kacSaat; i++) {
	        gununSaatHesabi(gunSaatSayisi);
	    }
	}
	
	public void gununSaatHesabi(int gun_uzunlugu) 
	{
		saat++;
		if ( saat>= gun_uzunlugu) 
		{
			saat=0;
			gun++;
			
			if (gun > 30) 
			{
				gun=1;
				ay++;
				
				if (ay>12) 
				{
					ay=1;
					yil++;
				}
			}
		}
	}
	
	public String toString() 
	{
		return String.format("%02d.%02d.%04d", gun, ay, yil);
	}
	
	
	
	public boolean cikisTarihiKontrol (zaman tarih) 
	{
		return this.gun == tarih.gun && this.ay == tarih.ay && this.yil == tarih.yil;
		// true: arac kalkacak || false: arac bekleyecek
	}
	
	
	
	
	
	
	


	public int getGun() {
		return gun;
	}


	public void setGun(int gun) {
		this.gun = gun;
	}


	public int getAy() {
		return ay;
	}


	public void setAy(int ay) {
		this.ay = ay;
	}


	public int getYil() {
		return yil;
	}


	public void setYil(int yil) {
		this.yil = yil;
	}


	public int getSaat() {
		return saat;
	}


	public void setSaat(int saat) {
		this.saat = saat;
	}

}
