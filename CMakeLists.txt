cmake_minimum_required(VERSION 3.8)
project( Matura )
find_package( OpenCV REQUIRED )
find_package( Corrade REQUIRED Main )
find_package( Magnum REQUIRED GL MeshTools Shaders SceneGraph Trade Sdl2Application AnySceneImporter )
find_package( MagnumPlugins REQUIRED GltfImporter )

set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)


include_directories( ${OpenCV_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ./ /usr/include/colmap )
add_executable( ${PROJECT_NAME} main.cpp SerialCom.h RadialCamera.h TextureShader.h Joystick.h )

target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} Corrade::Main Magnum::Application Magnum::GL Magnum::Magnum Magnum::MeshTools Magnum::SceneGraph Magnum::Shaders Magnum::Trade Magnum::AnySceneImporter MagnumPlugins::GltfImporter)

install(TARGETS ${PROJECT_NAME} DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})
add_compile_options( -std=c++20 )  