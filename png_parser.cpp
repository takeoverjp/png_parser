#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <iostream>

static const u_char PNG_SIG[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

static void
usage_exit ()
{
  std::cerr << "[USAGE] png_parser FILE" << std::endl;
  exit (EXIT_FAILURE);
}

int
main (int argc, char *argv[])
{
  if (argc != 2)
    usage_exit();

  const char* const file_name = argv[1];
  const int fd = open (file_name, O_RDONLY);
  if (fd < 0)
    {
      perror ("fread");
      exit (EXIT_FAILURE);
    }

  off_t size = lseek (fd, 0, SEEK_END);

  u_char* ptr = static_cast<u_char*>(mmap (nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));
  if (ptr == MAP_FAILED)
    {
      perror("mmap");
      exit (EXIT_FAILURE);
    }

  off_t offset = 0;

  bool is_png = !memcmp (ptr, PNG_SIG, sizeof(PNG_SIG));
  if (!is_png)
    {
      std::cerr << "parse error" << std::endl;
      exit (EXIT_FAILURE);
    }

  offset += sizeof(PNG_SIG);

  while (offset < size)
    {
      const uint32_t length = be32toh(reinterpret_cast<uint32_t *>(ptr + offset)[0]);
      const u_char * const type = ptr + offset + sizeof(length);
      const int chunk_bytes =
        sizeof(length)  // length
        + 4             // type
        + length        // chunk data
        + 4;            // CRC
      offset += chunk_bytes;

      std::cout << type[0] << type[1] << type[2] << type[3] << ", " << chunk_bytes << std::endl;

      if (!memcmp (type, "IEND", 4))
        break;
    }

  exit (EXIT_SUCCESS);
}
