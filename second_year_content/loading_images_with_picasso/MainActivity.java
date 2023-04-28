package com.example.usbee;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import com.example.usbee.database.Blabla;
import com.example.usbee.database.Databasetry;
import com.example.usbee.displayImg.ImageActivity;

public class MainActivity extends AppCompatActivity {
    TextView textView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        defineButtons();
    }

    // Code taken from https://github.com/JohnsAndroidStudioTutorials/MultipleButtons
    public void defineButtons() {
        findViewById(R.id.roomFinderActivityBtn).setOnClickListener(buttonClickListener);
        findViewById(R.id.roomSearchActivityBtn).setOnClickListener(buttonClickListener);
        findViewById(R.id.settingsActivityBtn).setOnClickListener(buttonClickListener);
        findViewById(R.id.databaseapp).setOnClickListener(buttonClickListener);
        textView=(TextView)findViewById(R.id.textView);


    }

    private View.OnClickListener buttonClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            switch (view.getId()) {
                case R.id.roomFinderActivityBtn:
                    Intent roomFinderIntent = new Intent(getApplicationContext(), RoomFinderActivity.class);
                    startActivity(roomFinderIntent);
                    break;

                case R.id.databaseapp:

                   Intent roomSearchIntent = new Intent(getApplicationContext(), Databasetry.class);
                    startActivity(roomSearchIntent);
                    break;

                // Uncomment once RoomSearchActivity has been created
               case R.id.roomSearchActivityBtn:
                /*Blabla lol = new Blabla();
                textView.append(" "+ lol.method(getApplicationContext()));*/
                   Intent ImageIntent = new Intent(getApplicationContext(), ImageActivity.class);
                   startActivity(ImageIntent);


//                    Intent roomSearchIntent = new Intent(getApplicationContext(), RoomSearchActivity.class);
//                    startActivity(roomSearchIntent);
                    break;

                // once settingsActivityBtn has been created
//                case R.id.settingsActivityBtn:
//                    Intent settingsIntent = new Intent(getApplicationContext(), settingsActivity.class);
//                    startActivity(settingsIntent);
//                    break;
            }
        }
    };
}
