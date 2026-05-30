package com.example.cauper

object CauperNativeLib {
    init {
        System.loadLibrary("cauper")
    }
    external fun setNativeSurface(surface: Any?): String?

    external fun initClient(): Int

    external fun connectClient(ip: String, port: Int): Int

    external fun deinitClient()

    external fun touch(x: Int, y: Int, is_down: Int)
    external fun toggleStream()
    external fun stopClient()
}
