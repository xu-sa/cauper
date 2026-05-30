package com.example.cauper

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Toast
import com.example.cauper.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        val i = CauperNativeLib.initClient()
        if (i != 1) {
            val str = "Init Failed,code: $i"
            Toast.makeText(this, str, Toast.LENGTH_SHORT).show()
            finish()
            return
        }

        binding.btnConnect.setOnClickListener {
            val ip = binding.etIp.text.toString()
            val portStr = binding.etPort.text.toString()

            if (ip.isEmpty() || portStr.isEmpty()) {
                Toast.makeText(this, "Please enter IP and Port", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }

            try {
                val port = portStr.toInt()
                val res = CauperNativeLib.connectClient(ip, port)
                if(res == 1) {
                    val intent = Intent(this, VideoActivity::class.java)
                    startActivity(intent)
                }else{
                    val str = "Failed to connect,code: $res"
                    Toast.makeText(this, str, Toast.LENGTH_SHORT).show()
                }
            } catch (e: Exception) {
                Toast.makeText(this, "Invalid Port", Toast.LENGTH_SHORT).show()
            }
        }

        binding.btnExit.setOnClickListener {
            CauperNativeLib.deinitClient()
            finish()
        }
    }
}
