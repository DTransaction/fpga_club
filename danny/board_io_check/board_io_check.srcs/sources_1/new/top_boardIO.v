`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Create Date: 05/07/2025 10:53:56 AM
// Created by: Danny Tran 
// 
// Purpose: 
// Ensure on-board buttons, switches, and LEDs are functional 
//
// Description: 
// LED 0-3 with turn on if its corresponding button pressed.
// RGB LEDs are controlled with the switches: 
//     sw[0] = RGB LED select (0 for LED5, 1 for LED6)
//     sw[1] = blue 
//     sw[2] = green
//     sw[3] = red 
// 
//////////////////////////////////////////////////////////////////////////////////

// This module maps the on-board IO to the boardIO module 
// The input and output variables are from the constraints file 
module top_boardIO(
    input [3:0] btn,
    input [3:0] sw,
    output [3:0] led,
    output led5_r,
    output led5_g,
    output led5_b,
    output led6_r,
    output led6_g,
    output led6_b
    );
    
    boardIO uut (
        .btn(btn),
        .sw(sw),
        .led(led),
        .rgb_1({led5_r, led5_g, led5_b}),
        .rgb_2({led6_r, led6_g, led6_b})
    ); 
endmodule
