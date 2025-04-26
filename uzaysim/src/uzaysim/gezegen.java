/**
*
* @author Aybuke Gokce Yavas aybuke.yavas@ogr.sakarya.edu.tr
* @since 20/04/2025
* <p>
* Gezegen sinifi
* </p>
*/

package uzaysim;

public class gezegen 
{
	private String gezegen_adi;
	private int gun_uzunlugu;
	private zaman gezegen_tarihi;
	private int nufus;
	
	
	public gezegen(String gezegen_adi, int gun_uzunlugu, zaman gezegen_tarihi) 
	{
		this.gezegen_adi = gezegen_adi;
		this.gun_uzunlugu = gun_uzunlugu;
		this.gezegen_tarihi = gezegen_tarihi;
	}


	public String getGezegen_adi() {
		return gezegen_adi;
	}


	public void setGezegen_adi(String gezegen_adi) {
		this.gezegen_adi = gezegen_adi;
	}


	public int getGun_uzunlugu() {
		return gun_uzunlugu;
	}


	public void setGun_uzunlugu(int gun_uzunlugu) {
		this.gun_uzunlugu = gun_uzunlugu;
	}


	public zaman getGezegen_tarihi() {
		return gezegen_tarihi;
	}


	public void setGezegen_tarihi(zaman gezegen_tarihi) {
		this.gezegen_tarihi = gezegen_tarihi;
	}
	
	public int getNufus() {
	    return nufus;
	}

	public void setNufus(int nufus) {
	    this.nufus = nufus;
	}

}
