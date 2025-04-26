/**
*
* @author Aybuke Gokce Yavas aybuke.yavas@ogr.sakarya.edu.tr
* @since 20/04/2025
* <p>
* Simulasyon Sinifi:
* </p>
*/

package uzaysim;

import java.util.ArrayList;

public class sim {
	
	// METOTLAR
	
	public static boolean program(ArrayList<uzayaraci> uzayaraclari) 
	{
		for(uzayaraci arac : uzayaraclari) 
		{
			if (!arac.isImha_durumu() && arac.isArac_durumu()) {
				return false;
			}
		}
		return true;
	}
	
	public static gezegen gezegeniBul(String gezegen_adi, ArrayList<gezegen> gezegenler) 
	{
		for (gezegen g : gezegenler) {
			if (g.getGezegen_adi().equals(gezegen_adi)) {
				return g;
			}
		}
		return null;
	}
	
	
	private static void clearScreen() {
	    try {
	        if (System.getProperty("os.name").contains("Windows")) {
	            new ProcessBuilder("cmd", "/c", "cls").inheritIO().start().waitFor();
	        } else {
	            System.out.print("\033[H\033[2J");
	            System.out.flush();
	        }
	    } catch (Exception e) {
	        for (int i = 0; i < 50; i++) {
	            System.out.println();
	        }
	    }
	}
	
	
	// MAIN
	
	public static void main(String[] args) 
	{
		String KISILER_DOSYASI = "data/Kisiler.txt";
		String GEZEGENLER_DOSYASI = "data/Gezegenler.txt";
		String ARACLAR_DOSYASI = "data/Araclar.txt";
		
		ArrayList<kisi> kisiler = dosyaokuma.kisilerioku(KISILER_DOSYASI);
		ArrayList<gezegen> gezegenler = dosyaokuma.gezegenlerioku(GEZEGENLER_DOSYASI);
		ArrayList<uzayaraci> uzayaraclari = dosyaokuma.uzayaraclarinioku(ARACLAR_DOSYASI, gezegenler);


		
		
		while (!program(uzayaraclari)) 
		{
			clearScreen(); 
			
			for (gezegen g : gezegenler) {
			    g.setNufus(0);
			}
			
			for (gezegen g : gezegenler) 
			{
				g.getGezegen_tarihi().gununSaatHesabi(g.getGun_uzunlugu());
			}
			
			for (kisi k : kisiler) {
			    uzayaraci bagliArac = null;
			    for (uzayaraci a : uzayaraclari) {
			        if (a.getUzay_araci_adi().equals(k.getBulundugu_uzay_araci_adi())) {
			            bagliArac = a;
			            break;
			        }
			    }

			    if (bagliArac != null) {
			        if (!bagliArac.isArac_durumu() && !bagliArac.isImha_durumu()) {
			            
			            gezegen cikisGezegen = gezegeniBul(bagliArac.getCikis_gezegeni(), gezegenler);
			            if (cikisGezegen != null) {
			                cikisGezegen.setNufus(cikisGezegen.getNufus() + 1);
			            }
			        } else if (!bagliArac.isArac_durumu() && bagliArac.getMesafe_saat() == 0) {
			           
			            gezegen varisGezegen = gezegeniBul(bagliArac.getVaris_gezegeni(), gezegenler);
			            if (varisGezegen != null) {
			                varisGezegen.setNufus(varisGezegen.getNufus() + 1);
			            }
			        }
			    }
			}
					
			//////////////////////////
			
			for (uzayaraci a : uzayaraclari) 
			{
				if (!a.isImha_durumu()) 
				{
					if(!a.isArac_durumu()) 
					{
						gezegen cikisGezegeni = gezegeniBul(a.getCikis_gezegeni(), gezegenler);
						
						if (cikisGezegeni.getGezegen_tarihi().cikisTarihiKontrol(a.getCikis_tarihi())) 
						{
							a.setArac_durumu(true);
						}
					} else 
						{
						if (a.getMesafe_saat() > 0) {
						    a.setMesafe_saat(a.getMesafe_saat() - 1);

						    if (a.getMesafe_saat() <= 0) {
						        a.setMesafe_saat(0);
						        a.setArac_durumu(false); 
						    }
						}

							
							if (a.getMesafe_saat()<=0) 
							{
								a.setArac_durumu(false);
							}
						
						}
				}
			}
			
			///////////////////////////////////////////////////
			
			ArrayList<kisi> olumler = new ArrayList<>();
			
			for (kisi k : kisiler) 
			{
				k.setKalan_omur(k.getKalan_omur() - 1);
				
				if (k.getKalan_omur()<=0) {
					olumler.add(k);
				}
			}
			
			kisiler.removeAll(olumler);
			
			for (uzayaraci arac : uzayaraclari) {
			    if (!arac.isImha_durumu() && arac.getMesafe_saat() == 0) {
			        gezegen varisGezegen = gezegeniBul(arac.getVaris_gezegeni(), gezegenler);

			        if (varisGezegen != null) {
			            for (kisi k : kisiler) {
			                if (k.getBulundugu_uzay_araci_adi().equals(arac.getUzay_araci_adi())) {
			                    varisGezegen.setNufus(varisGezegen.getNufus() + 1);
			                }
			            }
			        }
			    }
			}

			
			
			/////////////////////////////////////////////////////
			
			for (uzayaraci a : uzayaraclari) 
			{
				if(!a.isImha_durumu()) 
				{
					boolean yolcularOldu = true;
					for (kisi k: kisiler) {
						if (k.getBulundugu_uzay_araci_adi().equals(a.getUzay_araci_adi())) 
						{
							yolcularOldu = false;
							break;
						}
					}
					
					if(yolcularOldu) 
					{
						a.setImha_durumu(true);
						a.setArac_durumu(false);
						a.setVaris_tarihi(null);
					}
				}
				
				
			}
			///////////////////////////////////////////////////////////////////

			System.out.println("\nGezegenler:");
			System.out.printf("%-20s", "Gezegenler:");
			for (gezegen g : gezegenler) {
			    System.out.printf("%-20s", g.getGezegen_adi());
			}
			System.out.println();

			System.out.printf("%-20s", "Tarih:");
			for (gezegen g : gezegenler) {
			    System.out.printf("%-20s", g.getGezegen_tarihi().toString());
			}
			System.out.println();

			System.out.printf("%-20s", "Nüfus:");
			for (gezegen g : gezegenler) {
			    System.out.printf("%-20d", g.getNufus());
			}
			System.out.println();

			System.out.println();
			System.out.println();
			System.out.println();

			System.out.println("Uzay Araçları:");
			System.out.printf("%-20s %-20s %-15s %-15s %-20s %-20s\n",
			    "Araç Adı", "Durum", "Çikis", "Varis", "Hedefe Kalan Sure", "Varis Tarihi");

			for (uzayaraci a : uzayaraclari) {
			    String durum;
			    if (a.isImha_durumu()) {
			        durum = "IMHA";
			    } else if (a.isArac_durumu()) {
			        durum = "HAREKET HALINDE";
			    } else {
			        if (a.getMesafe_saat() == 0) {
			            durum = "VARDI";
			        } else {
			            durum = "BEKLIYOR";
			        }
			    }

			    String varisTarihi = "-";
			    if (a.getVaris_tarihi() != null) {
			        varisTarihi = a.getVaris_tarihi().toString();
			    }



			    String hedefeKalanSure;
			    if (a.isImha_durumu()) {
			        hedefeKalanSure = "--";
			    } else {
			        hedefeKalanSure = String.valueOf(a.getMesafe_saat());
			    }

			    System.out.printf("%-20s %-20s %-15s %-15s %-20s %-20s\n",
			        a.getUzay_araci_adi(), durum, a.getCikis_gezegeni(), a.getVaris_gezegeni(),
			        hedefeKalanSure, varisTarihi);

			}

		
			try {
			    Thread.sleep(500);
			} catch (InterruptedException e) {
			    e.printStackTrace();
			}


		}
		
		
	}

}
