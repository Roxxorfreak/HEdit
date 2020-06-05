// Viewer for ZIP Archives
main("ZIP Archive")
{ 
  gotoxy(2, 1);
  print("Zip file content: ");
  declare(line, true);
  declare(totalOffset, true);
  declare(fileLength, true);
  declare(extraLength, true);
  declare(cSize, false);
  declare(condition, true);
  set(line, 3); // Start in line 3
  loop;
    add(totalOffset, 18); // Offset of compressed file size
    loadd(cSize, totalOffset);
    gotoxy(50, line);
    printud("Comp.: ", " bytes", totalOffset);
    add(totalOffset, 4); // Offset of uncompressed file size
    gotoxy(80, line);
    printud("Uncomp.: ", " bytes", totalOffset);
    add(totalOffset, 4); // Offset of file name length
    loadw(fileLength, totalOffset);
    add(totalOffset, 2); // Offset of extra field length
    loadw(extraLength, totalOffset);
    add(totalOffset, 2); // Offset of file name 
    gotoxy(2, line);
    print("File: ");
    printds(totalOffset, fileLength);
    add(totalOffset, fileLength);
    add(totalOffset, extraLength);
    add(totalOffset, cSize);
    incr(line);
    // Load the id
    loadd(condition, totalOffset);
  until(condition, 0x02014b50);
  return;
}
