// Numeric Values
main ( "Numeric Value" )
{
  gotoxy(2, 2);
  print("Hex Value......: ");
  prinths(0, 4);
  gotoxy(2, 3); print("Checksum (xor).: ");
  printxor(0, 4);
  gotoxy(2, 5); printsb("Signed byte....: ", "", 0);
  gotoxy(2, 6); printsw("Signed word....: ", "", 0);
  gotoxy(2, 7); printsd("Signed dword...: ", "", 0);
  gotoxy(2, 8); printub("Unsigned byte..: ", "", 0);
  gotoxy(2, 9); printuw("Unsigned word..: ", "", 0);
  gotoxy(2, 10); printud("Unsigned dword.: ", "", 0);
  gotoxy(2, 11); printf("Floating point.: ", "", 0);
  gotoxy(2, 12); printd("Double.........: ", "", 0);
}
