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

module boardIO(
    input [3:0] btn,
    input [3:0] sw,
    output [3:0] led,
    output reg [2:0] rgb_1,
    output reg [2:0] rgb_2
    );
    assign led = btn; 
    always @ (sw) begin
        if (sw[0]) begin
            rgb_1 = sw[3:1];
            rgb_2 = 3'b000;
        end else begin
            rgb_2 = sw[3:1];
            rgb_1 = 3'b000;
        end
    end
endmodule
