/*
  Copyright (C) 2006 Alexander Petry

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include <ipna/capture/CaptureProgram.hpp>

#include <signal.h>
#include <stdlib.h>
#include <iostream>

#include <QCoreApplication>

ipna::capture::CaptureProgram program;

void stop() {
  program.stop();
}

void handleSignal(int sig) {
  exit(0);
}

int main(int argc, char** argv) {
  program.initialize(argc,argv);

  atexit(stop);
  signal(SIGTERM,handleSignal);
  signal(SIGINT,handleSignal);

  program.start();
}
