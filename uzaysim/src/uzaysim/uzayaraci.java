/**
*
* @author Aybuke Gokce Yavas aybuke.yavas@ogr.sakarya.edu.tr
* @since 20/04/2025
* <p>
* Uzay Araclari Sinifi
* </p>
*/

package uzaysim;

public class uzayaraci 
{
	private String uzay_araci_adi;
	private String cikis_gezegeni;
	private String varis_gezegeni;
	private zaman cikis_tarihi; 
	private zaman varis_tarihi;
	private int mesafe_saat;
	private boolean arac_durumu;
	private boolean imha_durumu;
	
	public uzayaraci(String uzay_araci_adi, String cikis_gezegeni, String varis_gezegeni, zaman cikis_tarihi, int mesafe_saat) 
	{
		this.uzay_araci_adi = uzay_araci_adi;
		this.cikis_gezegeni = cikis_gezegeni;
		this.varis_gezegeni = varis_gezegeni;
		this.cikis_tarihi = cikis_tarihi;
		this.mesafe_saat = mesafe_saat;
		this.arac_durumu = true;
	}
	
	public boolean hedefeVardi() 
	{
		return mesafe_saat == 0;
	}
	
	public zaman getVaris_tarihi() {
	    return varis_tarihi;
	}

	public void setVaris_tarihi(zaman varisTarihi) {
	    this.varis_tarihi = varisTarihi;
	}


	public String getUzay_araci_adi() {
		return uzay_araci_adi;
	}

	public void setUzay_araci_adi(String uzay_araci_adi) {
		this.uzay_araci_adi = uzay_araci_adi;
	}

	public String getCikis_gezegeni() {
		return cikis_gezegeni;
	}

	public void setCikis_gezegeni(String cikis_gezegeni) {
		this.cikis_gezegeni = cikis_gezegeni;
	}

	public String getVaris_gezegeni() {
		return varis_gezegeni;
	}

	public void setVaris_gezegeni(String varis_gezegeni) {
		this.varis_gezegeni = varis_gezegeni;
	}

	public zaman getCikis_tarihi() {
		return cikis_tarihi;
	}

	public void setCikis_tarihi(zaman cikis_tarihi) {
		this.cikis_tarihi = cikis_tarihi;
	}

	public int getMesafe_saat() {
		return mesafe_saat;
	}

	public void setMesafe_saat(int mesafe_saat) {
		this.mesafe_saat = mesafe_saat;
	}

	public boolean isArac_durumu() {
		return arac_durumu;
	}

	public void setArac_durumu(boolean arac_durumu) {
		this.arac_durumu = arac_durumu;
	}
	
	public boolean isImha_durumu() {
	    return imha_durumu;
	}

	public void setImha_durumu(boolean imha_durumu) {
	    this.imha_durumu = imha_durumu;
	}
	
	
}
