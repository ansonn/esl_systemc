#ifndef EXT_MEM_MODEL_H
#define EXT_MEM_MODEL_H

class ExtMemModel
{
public:

  // The constructor takes the required memory size (in bytes) as an argument
	ExtMemModel(unsigned int size_in_bytes);
	~ExtMemModel();

  // The reset functions puts the model into a known state
  // and clears the contents of its memory to 0x0
	void reset(void);

	//
	// Memory access functions
	//

	unsigned int read8(unsigned int addr);
	unsigned int read16(unsigned int addr);
	unsigned int read32(unsigned int addr);

	void write8(unsigned int addr, unsigned int data);
	void write16(unsigned int addr, unsigned int data);
	void write32(unsigned int addr, unsigned int data);

private:
	unsigned char * memory;
	unsigned int memory_size_in_bytes;
};

#endif
