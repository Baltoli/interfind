add_executable(interfind
  src/main.cpp
)

target_link_libraries(interfind
  ${llvm_libs}
  stdc++fs
  CONAN_PKG::fmt
  CONAN_PKG::jsonformoderncpp
)

install(
  TARGETS interfind
  DESTINATION ${CMAKE_INSTALL_BINDIR})
