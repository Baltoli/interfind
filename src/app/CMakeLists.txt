add_executable(interfind
  src/main.cpp
)

target_link_libraries(interfind
  ${llvm_libs}
  CONAN_PKG::jsonformoderncpp
)

install(
  TARGETS interfind
  DESTINATION ${CMAKE_INSTALL_BINDIR})
