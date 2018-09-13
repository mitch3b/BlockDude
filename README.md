# Block Dude port for the NES

## Overview

This is a port of the TI-83 (and other calcs) puzzle game Block Dude. In the game, you control Block Dude, whose powers are to pick up and put down one block at a time. He can also climb one block at a time! Using the blocks, you make your way to the door which brings you to the next level. The original was created by Brandon Sterner and can be found [here](https://education.ti.com/af/software/details/en/6AD0564FC779423E90F3BE776F72B5FD/83puzzlepack). There are also great emulators if you don't have the calculator yourself. OR if you have a way to play NES roms, you can play this port :)

## Why

I'm still in the process of learning to develop NES games. I first wrote a NES tool in assembly for calculating mash speed called MashyMashy and wanted to go through the process of building a game without being bogged down by creative decisions, intense featuresets or complex physics. Block Dude was always fun (mostly because you could play it in class) and was the perfect fit.

## Deviations

A lot of the screens outside of the actual game are significantly different, for example, the help screen has updated buttons and the title screen has a pretty different look. I also created completely original music to make the experience of thinking through a level a little more bearable. I haven't done any intense analysis on how different the physics are, but I tried comparing them in slow motion to get everything as close as I could.

## For devs

This code is a mess. I started with one of the nesdoug.com projects and built it from there so a lot of the NES specific code I didn't write or barely touched (and honestly should understand better). Also, the folks on the nesdev.com forum were extremely helpful. Otherwise, if you want to build this yourself and build off it, the hardest part will be the initial build. There's a makefile for linux/mac and a compile.bat file for windows. I used both throughout the process. It's also important to note I'm using cc65 V2.16. Syntax seems to vary dramatically for other versions so be careful. I had ideas for improvements, but there are a lot of limitations about making everything sized one sprite (specifically around color) and I couldn't reach the original author to get opinions about it. So all I ask is that if you do anything with this code, be respectful to Brandon because as far as I understand, Block Dude is his character.

