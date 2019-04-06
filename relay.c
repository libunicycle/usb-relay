// SPDX-License-Identifier: MIT

#include <libopencm3/stm32/desig.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <librfn/console.h>
#include <librfn/fibre.h>
#include <librfn/time.h>
#include <librfn/util.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static console_t cdcacm_console;

static pt_state_t do_id(console_t *c) {
    char serial_no[25];

    desig_get_unique_id_as_string(serial_no, sizeof(serial_no));
    fprintf(c->out, "%s\n", serial_no);

    return PT_EXITED;
}

static pt_state_t do_version(console_t *c) {
    fprintf(c->out, "USB relay made by Anatol Pomozov (anatol.pomozov@gmail.com) %s at %s\n", __DATE__, __TIME__);
    return PT_EXITED;
}

static pt_state_t do_uptime(console_t *c) {
    unsigned int hours, minutes, seconds, microseconds;

    uint64_t t = time64_now();

    /* get to 32-bit values as directly as possible */
    minutes = t / (60 * 1000000);
    microseconds = t % (60 * 1000000);

    hours = minutes / 60;
    minutes %= 60;
    seconds = microseconds / 1000000;
    microseconds %= 1000000;

    fprintf(c->out, "%02u:%02u:%02u.%03u\n", hours, minutes, seconds, microseconds / 1000);

    return PT_EXITED;
}

static const console_cmd_t cmds[] = {CONSOLE_CMD_VAR_INIT("id", do_id), CONSOLE_CMD_VAR_INIT("uptime", do_uptime),
                                     CONSOLE_CMD_VAR_INIT("version", do_version)};

#define ON_WITH_OPEN_DRAIN (console_gpio_default_on | console_gpio_open_drain)

const console_gpio_t gpios[] = {
    CONSOLE_GPIO_VAR_INIT("powerbtn", GPIOB, GPIO11, ON_WITH_OPEN_DRAIN),
    CONSOLE_GPIO_VAR_INIT("resetbtn", GPIOB, GPIO10, ON_WITH_OPEN_DRAIN),

    CONSOLE_GPIO_VAR_INIT("led", GPIOC, GPIO13, console_gpio_active_low),
};

int main(void) {
#ifdef STM32F4
    rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_120MHZ]);
#endif
#ifdef STM32F1
    rcc_clock_setup_in_hsi_out_48mhz();
#endif

    time_init();
    console_init(&cdcacm_console, stdout);
    cdcacm_console.prompt = "";

    for (unsigned int i = 0; i < lengthof(cmds); i++)
        console_register(&cmds[i]);
    for (unsigned int i = 0; i < lengthof(gpios); i++)
        console_gpio_register(&gpios[i]);

    fibre_scheduler_main_loop();

    return 0;
}
