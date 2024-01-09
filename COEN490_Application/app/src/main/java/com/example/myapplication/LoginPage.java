package com.example.myapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

public class LoginPage extends AppCompatActivity {

    EditText emailEditText, passwordEditText;

    Button btnLogIn, btnCreateAcc, btnForgotPass;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_loginpage);

        btnForgotPass = findViewById(R.id.btnForgotPass);
        btnCreateAcc = findViewById(R.id.btnCreateAcc);


        btnForgotPass.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(LoginPage.this, ForgotPassword.class);
                startActivity(i);
            }
        });

        btnCreateAcc.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = new Intent(LoginPage.this, CreateAccount.class);
                startActivity(i);
            }
        });

    }
}