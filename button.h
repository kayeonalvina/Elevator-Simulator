#ifndef BUTTON_H
#define BUTTON_H


class Button
{
public:
    Button();

    bool getStatus();
    void setStatus(bool status);

    private:
        bool status; // true - on; false - off
};

#endif // BUTTON_H
