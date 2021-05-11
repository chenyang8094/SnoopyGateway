set(SOURCE_FILES_SRC
   CmdService.cpp
   Log.cpp 
   main.cpp 
   RestApi.cpp 
   )

add_executable(SnoopyGateway ${SOURCE_FILES_SRC})
target_link_libraries(SnoopyGateway pthread sensor device wiringPi wiringPiDev uv_a zlog dl)
