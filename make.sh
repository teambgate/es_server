#
# Copyright (C) 2017 Manh Tran
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
mkdir -p build
cmake -Bbuild -DOUT=build -H. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DAPP_DEBUG=true
make -C build
cp -rf res build
