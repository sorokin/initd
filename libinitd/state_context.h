#ifndef STATE_CONTEXT_H
#define STATE_CONTEXT_H

struct state_context
{
    virtual void reboot() = 0;
    virtual void power_off() = 0;
};

#endif
