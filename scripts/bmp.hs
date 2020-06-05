// Viewer for Bitmap Headers
main("Bitmap") 
{ 
  gotoxy(2, 1);
  print("Windows - Bitmap");
  gotoxy(2, 3);
  printud("Width.......: ", " Pixel", 18);
  gotoxy(2, 4);
  printud("Height......: ", " Pixel", 22);
  gotoxy(2, 5);
  printuw("Color depth.: ", " Bit", 28);
  return;
}

