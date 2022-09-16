cmake_minimum_required(VERSION 3.8)
project( Matura )
add_executable( ${PROJECT_NAME} main.cpp SerialCom.h RadialCamera.h TextureShader.h Joystick.h PhongDrawable.h )


find_package( OpenCV REQUIRED )
find_package( Corrade REQUIRED Main )
find_package( Magnum REQUIRED GL MeshTools Shaders SceneGraph Trade Sdl2Application AnySceneImporter )
find_package( MagnumPlugins REQUIRED GltfImporter )
find_package( MagnumIntegration REQUIRED ImGui )





set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)


include_directories( ${OpenCV_INCLUDE_DIRS} ./ /usr/include/colmap /usr/include/MagnumExternal/ImGui/ )


target_link_libraries( ${PROJECT_NAME} PRIVATE ${OpenCV_LIBS} )
target_link_libraries(${PROJECT_NAME} PRIVATE Corrade::Main Magnum::Application Magnum::GL Magnum::Magnum Magnum::MeshTools Magnum::SceneGraph Magnum::Shaders Magnum::Trade Magnum::AnySceneImporter MagnumPlugins::GltfImporter MagnumIntegration::ImGui)


install(TARGETS ${PROJECT_NAME} DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})
add_compile_options( -std=c++20 -g )  