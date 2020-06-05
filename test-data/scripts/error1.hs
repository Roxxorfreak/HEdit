// Calculate the (XOR) checksum of a hex string
main ( "Checksum (XOR) with errors to test if error handling works" )
{
  gotoxy(0x2, 2);
  print("Hex String.: ");
  prinths(0, 4);
  gotoxy(2, 0x3t);
  print("Checksum...: ");
  printxor(0, 4);
}
