/**
*
* @author Aybuke Gokce Yavas aybuke.yavas@ogr.sakarya.edu.tr
* @since 20/04/2025
* <p>
* txt dosyalarini okuma sinifi
* </p>
*/

package uzaysim;

import java.io.*;
import java.util.ArrayList;

public class dosyaokuma 
{
	public static gezegen gezegeniBul(String gezegenAdi, ArrayList<gezegen> gezegenler) {
	    for (gezegen g : gezegenler) {
	        if (g.getGezegen_adi().equals(gezegenAdi)) {
	            return g;
	        }
	    }
	    return null;
	}

	//KISILER DIZISI
	
	public static ArrayList<kisi> kisilerioku(String dosyaAdi) 
	{
		ArrayList<kisi> kisiler = new ArrayList<>();  //return edilecek liste
		
		
		try (BufferedReader br = new BufferedReader(new FileReader(dosyaAdi)))
		{
			String satir;
			while((satir = br.readLine()) !=null) 
			{
				String[] parca = satir.split("#");
				
				if (parca.length ==4) 
				{
					String isim = parca[0];
					int yas = Integer.parseInt(parca[1]);
					int kalan_omur = Integer.parseInt(parca[2]);
					String bulundugu_uzay_araci_adi = parca[3];
					
					kisi kisi = new kisi(isim, yas, kalan_omur, bulundugu_uzay_araci_adi);
					kisiler.add(kisi);
				}
			}
			
		}
		catch (IOException e) 
		{
			System.out.println("Kisiler dosyası okunamadı:" + e.getMessage());
		}
		return kisiler;
	}
	
	
	
	
	// GEZEGENLER DIZISI
	
	public static ArrayList<gezegen> gezegenlerioku(String dosyaAdi) 
	{
		ArrayList<gezegen> gezegenler = new ArrayList<>();
		
		try (BufferedReader br = new BufferedReader(new FileReader(dosyaAdi))) 
		{
			String satir;
			while ((satir = br.readLine()) !=null)
			{
				String [] parca = satir.split("#");
				
				if (parca.length ==3) 
				{
					String gezegen_adi = parca [0];
					int gun_uzunlugu = Integer.parseInt(parca [1]);
					
					String[] tarihSatırı = parca[2].split("\\.");
					int gun = Integer.parseInt(tarihSatırı[0]);
					int ay = Integer.parseInt(tarihSatırı[1]);
					int yil = Integer.parseInt(tarihSatırı[2]);
					zaman gezegen_tarihi = new zaman(gun, ay, yil);
					
					gezegen gezegen = new gezegen(gezegen_adi, gun_uzunlugu, gezegen_tarihi);
					gezegenler.add(gezegen);
					
				}
			}
		
		}
		catch (IOException e) 
		{
			System.out.println("Gezegenler dosyası okunamadı:" + e.getMessage());
		}
		
		return gezegenler;
	}
	
	
	// UZAY ARACLARI DIZISI
	
	public static ArrayList<uzayaraci> uzayaraclarinioku(String dosyaAdi, ArrayList<gezegen> gezegenler)
	{
		ArrayList<uzayaraci> uzayaraclari = new ArrayList<>();
		
		try (BufferedReader br = new BufferedReader(new FileReader(dosyaAdi)))
		{
			String satir;
			while ((satir = br.readLine()) !=null) 
			{
				String [] parca = satir.split("#");
				
				if (parca.length ==5) 
				{
				    String uzay_araci_adi = parca[0];
				    String cikis_gezegeni = parca[1];
				    String varis_gezegeni = parca[2];
				    
				    String[] tarihSatırı = parca[3].split("\\.");
				    int gun = Integer.parseInt(tarihSatırı[0]);
				    int ay = Integer.parseInt(tarihSatırı[1]);
				    int yil = Integer.parseInt(tarihSatırı[2]);
				    zaman cikis_tarihi = new zaman(gun, ay, yil);
				    
				    int mesafe_saat = Integer.parseInt(parca[4]);
				    
				    uzayaraci uzayaraci = new uzayaraci(uzay_araci_adi, cikis_gezegeni, varis_gezegeni, cikis_tarihi, mesafe_saat);

				    // VARIŞ TARİHİ HESAPLAMA
				    gezegen hedefGezegen = dosyaokuma.gezegeniBul(varis_gezegeni, gezegenler);
				    
				    if (hedefGezegen != null) {
				        zaman varisTarihi = cikis_tarihi.kopyala();
				        varisTarihi.saatleriIlerle(mesafe_saat, hedefGezegen.getGun_uzunlugu());
				        uzayaraci.setVaris_tarihi(varisTarihi);
				    }
				    
				    uzayaraclari.add(uzayaraci);
				}

			}
			
		}
		catch (IOException e) 
		{
			System.out.println("UzayAraclari dosyası okunamadı:" + e.getMessage());
		}
		return uzayaraclari;
	}


	

}
