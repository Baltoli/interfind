add_executable(interfind
  src/main.cpp
)

target_link_libraries(interfind
  ${llvm_libs}
)

install(
  TARGETS interfind
  DESTINATION ${CMAKE_INSTALL_BINDIR})
