#ifndef __BUZZER_PWM_MACHINE__H__
#define __BUZZER_PWM_MACHINE__H__

#ifdef __cplusplus
extern "C" {
#endif
int buzzer_init(void* tid);
void buzzer_thread(void);

void play_beep_once(void);
void play_funkytown_once(void);
void play_mario_once(void);
void play_golioth_once(void);
void play_double_beep_once(void);
#ifdef __cplusplus
}
#endif

#endif
