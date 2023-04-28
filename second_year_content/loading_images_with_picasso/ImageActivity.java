package com.example.usbee;

import android.database.Cursor;
import android.database.SQLException;
import android.database.sqlite.SQLiteDatabase;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

import com.example.usbee.database.DatabaseHelper;
import com.squareup.picasso.Picasso;

import java.io.IOException;

public class ImageActivity extends AppCompatActivity {
    ImageView imageView;

    private DatabaseHelper mDBHelper;
    private SQLiteDatabase mDb;
    Button btnviewAll;
    EditText edtEditText;
    String url;
    String content;
    String qq;


    public void showMessage(String title,String Message){
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setCancelable(true);
        builder.setTitle(title);
        builder.setMessage(Message);
        builder.show();
    }


    public void imageDisplay(){
        btnviewAll.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {

        content = edtEditText.getText().toString();

         qq = "SELECT Content FROM MultimediaContent WHERE RoomNumber LIKE ?";
         Cursor cursor = mDb.rawQuery(qq, new String[] {content});

       if(cursor.getCount() == 0) {
            showMessage("Error","Error Room Does Not Exist");

            return;
        }else{

            StringBuffer buffer = new StringBuffer();
            while (cursor.moveToNext()) {
                url = cursor.getString(0);
                buffer.append(cursor.getString(0));

            }
           Picasso.get()
                   .load(url)
                   .into(imageView);
           return;
        }
                    }
                }
        );
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_image);
        imageView = (ImageView)findViewById(R.id.imageView);
        edtEditText=(EditText)findViewById(R.id.editTextImg);
        btnviewAll = (Button)findViewById(R.id.imgButton);
        mDBHelper = new DatabaseHelper(this);
        try {
            mDBHelper.updateDataBase();
        } catch (IOException mIOException) {
            throw new Error("UnableToUpdateDatabase");
        }

        try {
            mDb = mDBHelper.getWritableDatabase();
        } catch (SQLException mSQLException) {
            throw mSQLException;
        }

        imageDisplay();

    }
}
