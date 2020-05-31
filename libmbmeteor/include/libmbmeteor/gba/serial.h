// This file is part of libmbmeteor.
// Copyright (C) 2019 Buenia.
//
// libmbmeteor is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libmbmeteor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libmbmeteor.  If not, see <https://www.gnu.org/licenses/>.

#ifndef LIBMBMETEOR_SERIAL
#define LIBMBMETEOR_SERIAL

#include "mmu.h"
#include <functional>
using namespace gba;
using namespace std;

namespace gba
{
    using writefunc = function<void(uint16_t, uint16_t)>;
    
    class LIBMBMETEOR_API PowerAntennaInterface
    {
        public:
            PowerAntennaInterface()
            {
            
            }
            
            ~PowerAntennaInterface()
            {
            
            }
            
            virtual void ledoff() = 0;
            virtual void ledonstrong() = 0;
            virtual void ledonweak() = 0;
    };
    
    class LIBMBMETEOR_API PowerAntenna
    {
        public:
            PowerAntenna()
            {
            
            }
            
            ~PowerAntenna()
            {
            
            }
            
    	    PowerAntennaInterface *inter = NULL;
    	    
    	    void setinterface(PowerAntennaInterface *cb)
    	    {
    	        inter = cb;
    	    }
            
            void ledoff()
    	    {
    	        if (inter != NULL)
    	        {
    	            inter->ledoff();
    	        }
    	    }
    	    
    	    void ledonstrong()
    	    {
    	        if (inter != NULL)
    	        {
    	            inter->ledonstrong();
    	        }
    	    }
    	    
    	    void ledonweak()
    	    {
    	        if (inter != NULL)
    	        {
    	            inter->ledonweak();
    	        }
    	    }
            
            void powerwrite(uint16_t rcnt, uint16_t siocnt)
            {
                if (!TestBit(rcnt, 15) && !TestBit(siocnt, 13) && TestBit(siocnt, 7))
                {
                    if (!TestBit(siocnt, 0))
                    {
               	 ledoff();
                    }
                    else if (TestBit(siocnt, 3))
                    {
			 ledonstrong();
                    }
                    else
                    {
                        ledonweak();
                    }
                }
            }
    };

    class LIBMBMETEOR_API Serial
    {
	public:
	    Serial(MMU& memory);
	    ~Serial();

	    MMU& serialmem;
	    
	    writefunc readywrite;
	    
	    void setwritecallback(writefunc cb)
	    {
	        readywrite = cb;
	    }

	    uint8_t readserial(uint32_t addr);
	    void writeserial(uint32_t addr, uint8_t val);

	    uint16_t rcnt = 0;
	    uint16_t siocnt = 0;
	    uint16_t siomltsend = 0;

	    enum SerialMode : int
	    {
		Unknown = -1,
		Normal8 = 0,
		Normal32 = 1,
		Multiplayer = 2,
		UART = 3,
		GeneralPurpose = 4,
		JOYBUS = 5
	    };

	    SerialMode mode = SerialMode::Unknown;

	    void updatemode()
	    {
	        if (readywrite)
	        {
		    readywrite(rcnt, siocnt);
		}
	    
		if (TestBit(rcnt, 15))
		{
		    if (TestBit(rcnt, 14))
		    {
			mode = SerialMode::JOYBUS;
		    }
		    else
		    {
			mode = SerialMode::GeneralPurpose;
		    }
		}
		else
		{
		    if (TestBit(siocnt, 13))
		    {
			if (TestBit(siocnt, 12))
			{
			    mode = SerialMode::UART;
			}
			else
			{
			    mode = SerialMode::Multiplayer;
			}
		    }
		    else
		    {
			if (TestBit(siocnt, 12))
			{
			    mode = SerialMode::Normal32;
			}
			else
			{
			    mode = SerialMode::Normal8;
			}
		    }
		}
	    }

	    void printregs()
	    {
		if (mode == SerialMode::Multiplayer)
		{
		    serialmem.memarm.printregs();
		    cout << endl;
		}
	    }
    };
};

#endif // LIBMBMETEOR_SERIAL
