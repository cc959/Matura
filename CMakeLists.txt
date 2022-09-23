cmake_minimum_required(VERSION 3.8)
project( Matura )

set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

find_package( OpenCV REQUIRED )
find_package( Corrade REQUIRED Main Utility )
find_package( Magnum REQUIRED GL MeshTools Shaders SceneGraph Trade Sdl2Application AnySceneImporter )
find_package( MagnumPlugins REQUIRED GltfImporter )
find_package( MagnumIntegration REQUIRED ImGui )

file(GLOB SourceFiles CONFIGURE_DEPENDS "src/*.cpp")
corrade_add_resource(Resources resources.conf)

include_directories( ${OpenCV_INCLUDE_DIRS} ./ /usr/include/colmap /usr/include/MagnumExternal/ImGui/ ./include/ )


add_executable( ${PROJECT_NAME} ${SourceFiles} ${Resources} )

target_link_libraries( ${PROJECT_NAME} PRIVATE ${OpenCV_LIBS} )
target_link_libraries(${PROJECT_NAME} PRIVATE Corrade::Main Magnum::Application Magnum::GL Magnum::Magnum Magnum::MeshTools Magnum::SceneGraph Magnum::Shaders Magnum::Trade Magnum::AnySceneImporter MagnumPlugins::GltfImporter MagnumIntegration::ImGui)

add_compile_options( -std=c++20 -g )