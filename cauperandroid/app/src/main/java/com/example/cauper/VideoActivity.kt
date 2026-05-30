package com.example.cauper

import android.annotation.SuppressLint
import android.graphics.PixelFormat
import android.os.Bundle
import android.view.MotionEvent
import android.view.View
import android.view.SurfaceHolder
import androidx.appcompat.app.AppCompatActivity
import com.example.cauper.databinding.ActivityVideoBinding
import android.widget.Toast

class VideoActivity : AppCompatActivity(), SurfaceHolder.Callback {

    private lateinit var binding: ActivityVideoBinding
    private var lastClickTime: Long = 0

    @SuppressLint("ClickableViewAccessibility")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        // 开启全屏沉浸模式
        window.decorView.systemUiVisibility = (View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                or View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                or View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                or View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                or View.SYSTEM_UI_FLAG_FULLSCREEN)

        binding = ActivityVideoBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // 设置侧边栏触发器的双击逻辑
        binding.btnSidebarTrigger.setOnClickListener {
            val currentTime = System.currentTimeMillis()
            if (currentTime - lastClickTime < 300) { // 300毫秒内连续点击判定为双击
                toggleSidebar()
            }
            lastClickTime = currentTime
        }

        // 设置 SurfaceView 的触摸监听，调用 JNI touch 函数
        binding.surfaceView.setOnTouchListener { v, event ->
//            val x = event.x.toInt()
//            val y = event.y.toInt()
            val width = v.width
            val height = v.height
            val x = if (width > 0) ((event.x / width) * 1000).toInt().coerceIn(0, 1000) else 0
            val y = if (height > 0) ((event.y / height) * 1000).toInt().coerceIn(0, 1000) else 0


            when (event.action) {
                MotionEvent.ACTION_DOWN -> {
                    CauperNativeLib.touch(x, y, 1)
                }
                MotionEvent.ACTION_MOVE -> {
                    CauperNativeLib.touch(x, y, 1)
                }
                MotionEvent.ACTION_UP, MotionEvent.ACTION_CANCEL -> {
                    CauperNativeLib.touch(x, y, 0)
                }
            }
            // 返回 true 表示消费了事件，允许接收后续的 MOVE 和 UP
            true
        }

        binding.surfaceView.holder.setFormat(PixelFormat.RGBA_8888)
        binding.surfaceView.holder.addCallback(this)

        // 按钮逻辑
        binding.btnStopVideo.setOnClickListener {
            CauperNativeLib.stopClient()
            finish()
        }

        binding.btnToggleVideo.setOnClickListener {
            CauperNativeLib.toggleStream()
        }
    }

    private fun toggleSidebar() {
        if (binding.layoutSidebar.visibility == View.VISIBLE) {
            binding.layoutSidebar.visibility = View.GONE
        } else {
            binding.layoutSidebar.visibility = View.VISIBLE
        }
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        val surface = holder.surface
        if (surface == null || !surface.isValid) return
        val result = CauperNativeLib.setNativeSurface(surface)
        if (result != null) {
            Toast.makeText(this, result, Toast.LENGTH_SHORT).show()
        }
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {}

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        CauperNativeLib.stopClient()
        CauperNativeLib.setNativeSurface(null)
    }
}
