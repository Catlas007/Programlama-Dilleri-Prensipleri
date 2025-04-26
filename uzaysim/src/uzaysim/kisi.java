/**
*
* @author Aybuke Gokce Yavas aybuke.yavas@ogr.sakarya.edu.tr
* @since 20/04/2025
* <p>
* Kisi Sinifi
* </p>
*/

package uzaysim;

public class kisi 
{
	private String isim;
	private int yas;
	private int kalan_omur;
	private String bulundugu_uzay_araci_adi;
	
	public kisi(String isim, int yas, int kalan_omur, String bulundugu_uzay_araci_adi) 
	{
		this.isim = isim;
		this.yas = yas;
		this.kalan_omur = kalan_omur;
		this.bulundugu_uzay_araci_adi = bulundugu_uzay_araci_adi;
	}

	public String getIsim() {
		return isim;
	}

	public void setIsim(String isim) {
		this.isim = isim;
	}

	public int getYas() {
		return yas;
	}

	public void setYas(int yas) {
		this.yas = yas;
	}

	public int getKalan_omur() {
		return kalan_omur;
	}

	public void setKalan_omur(int kalan_omur) {
		this.kalan_omur = kalan_omur;
	}

	public String getBulundugu_uzay_araci_adi() {
		return bulundugu_uzay_araci_adi;
	}

	public void setBulundugu_uzay_araci_adi(String bulundugu_uzay_araci_adi) {
		this.bulundugu_uzay_araci_adi = bulundugu_uzay_araci_adi;
	}

}
