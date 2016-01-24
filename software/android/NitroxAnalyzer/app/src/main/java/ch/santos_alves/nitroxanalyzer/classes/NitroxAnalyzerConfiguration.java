package ch.santos_alves.nitroxanalyzer.classes;

import java.util.Date;

/**
 * The Device configuration data structure stored in device eeprom
 *
 * Created by Sergio on 24.01.2016.
 */
public class NitroxAnalyzerConfiguration {
    private O2CellConfiguration o2Cell;

    /**
     * constructor
     */
    public NitroxAnalyzerConfiguration() {
        o2Cell = new O2CellConfiguration();
    }

    /**
     * Gets o2 cell configuration
     * @return the o2 cell configuration object
     */
    public O2CellConfiguration getO2Cell() {
        return o2Cell;
    }

    /**
     * The O2 Cell Configuration fields stored in device eeprom
     */
    public static class O2CellConfiguration {
        private Date installDate;
        private Date validityDate;


        public Date getInstallDate() {
            return installDate;
        }

        public void setInstallDate(Date installDate) {
            this.installDate = installDate;
        }

        public Date getValidityDate() {
            return validityDate;
        }

        public void setValidityDate(Date validityDate) {
            this.validityDate = validityDate;
        }
    }
}
