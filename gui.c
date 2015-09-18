#include <stdio.h>

// So I can't really do this here but this is what I was thinking.

int
gui_button(int x, int y, int width, int height, char* msg)
{
    // I could have this return 
}

if (gui_button(10, 10, 10, 10, "hello")) {
    alert("hello");
}


// This isn't exactly "retained" mode because all the state is still internal to the gui processing
// but it lets you respond to events without having to pass a callback. The problem though is that
// you will have 1 frame of lag when you do this because you really need to buffer stuff up to really know what is happening.
// You can have a gui_pump_events call and then handle inputs after that but in the same frame
// if you want to or you can handle them the next frame so stuff like what happens when you click
// a button still happen inline.

int slider = gui_slider(&number, 0, 100);
for (event* e = gui_next_event(slider);
     e != NULL;
     gui_next_event(e)) {
    // Iterate through the events for this slider this frame.
    // How do you make sure the event id is the same accross frames tho? I guess it doesn't
    // really have to be?
}
