package ch.santos_alves.nitroxanalyzer.activities;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import ch.santos_alves.nitroxanalyzer.R;

public class O2CellActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setTitle("Analyzer informations");
        setContentView(R.layout.activity_o2_cell);
        Bundle bundle = getIntent().getExtras();

        if (bundle.getString("installDate") != null) {
            ((TextView)findViewById(R.id.txtInstallDate)).setText(bundle.getString("installDate"));
        }
        if (bundle.getString("validityDate") != null) {
            ((TextView)findViewById(R.id.txtValidUntil)).setText(bundle.getString("validityDate"));
        }
    }
}
