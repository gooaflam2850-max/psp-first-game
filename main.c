#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>

// تعريف معلومات الموديول الخاص باللعبة (اسم اللعبة، الخصائص)
PSP_MODULE_INFO("First PSP Game", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

// دالة لمعالجة خروج اللاعب من اللعبة بلطف (تجنباً لحدوث تجميد في النظام)
int exit_callback(int arg1, int arg2, int common) {
    sceKernelExitGame();
    return 0;
}

int callback_thread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int setup_callbacks(void) {
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if (thid >= 0) {
        sceKernelStartThread(thid, 0, 0);
    }
    return thid;
}

int main(void) {
    // تهيئة شاشة التصحيح لعرض النصوص على شاشة الـ PSP
    pspDebugScreenInit();
    
    // تفعيل نظام الخروج الآمن
    setup_callbacks();

    pspDebugScreenPrintf("===================================\n");
    pspDebugScreenPrintf("   مرحباً بك في عالم تطوير ألعاب الـ PSP!\n");
    pspDebugScreenPrintf("===================================\n\n");
    pspDebugScreenPrintf("اضغط على زر (X) لطبع رسالة على الشاشة.\n");
    pspDebugScreenPrintf("اضغط على زر (START) للخروج من اللعبة.\n\n");

    // هيكل لتخزين حالة الأزرار
    SceCtrlData pad;
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    // حلقة اللعبة الرئيسية (Game Loop)
    while(1) {
        // قراءة المدخلات من أزرار الـ PSP
        sceCtrlReadBufferPositive(&pad, 1);

        // إذا ضغط اللاعب على زر START يتم إنهاء اللعبة
        if (pad.Buttons & PSP_CTRL_START) {
            break;
        }

        // إذا ضغط اللاعب على زر X
        if (pad.Buttons & PSP_CTRL_CROSS) {
            pspDebugScreenPrintf("تم الضغط على زر X بنجاح!\n");
            // تأخير بسيط لمنع تكرار قراءة الزر بسرعة كبيرة
            sceKernelDelayThread(200000); 
        }

        // تأخير خفيف داخل الحلقة لتقليل استهلاك معالج الجهاز
        sceKernelDelayThread(10000);
    }

    sceKernelExitGame();
    return 0;
}
