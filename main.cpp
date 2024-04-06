#include <iostream>
#include <fstream>
#include <memory>

// N < 256 и a_i < 256, для всех i=1..N
// .txt - массив данных в формате N и a_i, где i=1..N (ASCII)
// .bin - массив данных в формате N и a_i, где i=1..N (bin)

class DataReader
{
protected:
	std::fstream m_file;
	std::string m_filename;
	uint8_t* m_data;
	uint8_t m_n;

public:
	DataReader(const std::string& filename) : 
		m_filename(filename), m_n(0), m_data(nullptr) {}

	virtual ~DataReader() {	
        if(m_data != nullptr)
		delete[] m_data;
    }

	virtual bool Open() = 0;

	void Close()
	{
		m_file.close();
	}

	virtual void Read() = 0;

	virtual void Write() = 0;

	void GetData(uint8_t* buf, uint8_t& n)
	{
		n = m_n;
		for (int i = 0; i < m_n; i++)
			buf[i] = m_data[i];
	}
};

class TxtReader : public DataReader
{
public:
	TxtReader(const std::string& filename) : DataReader(filename) {}
	virtual ~TxtReader() {}

	bool Open() override
	{
		m_file.open(m_filename);
		return m_file.is_open();
	}

	void Read() override
	{
        uint8_t old_n = m_n;
        m_file >> m_n;
        m_n *= 4; // sizeof(int)

        if (m_n > old_n) {
            delete[] m_data;
            m_data = new uint8_t[m_n];
        } else if (!m_data && m_n != 0) {
            m_data = new uint8_t[m_n];
        }

        for (size_t i = 0; i < m_n; i += 4 ) {
            m_file >> *(int*)(m_data + i);
        }

	}

	void Write() override
	{

	}
};

class BinFReader : public DataReader
{
public:
	BinFReader(const std::string& filename) : DataReader(filename) {}
	virtual ~BinFReader()
	{
		if (m_data != nullptr)
			delete[] m_data;
	}

	bool Open() override
	{
		m_file.open(m_filename, std::ios::binary | std::ios::in);
		return m_file.is_open();
	}

	void Read() override
	{
        uint8_t old_n = m_n;
        m_file.read((char*)&m_n, 1);
        m_n *= 4;

		if (m_n > old_n) {
            delete[] m_data;
            m_data = new uint8_t[m_n];
        } else if (!m_data && m_n != 0) {
            m_data = new uint8_t[m_n];
        }

       m_file.read((char*)m_data, m_n);
	}

	void Write() override
	{

	}
};

void fill_file(const std::string& filename) {
    std::fstream file(filename, std::ios::out | std::ios::binary);
    const uint8_t n = 3;

    float buff[n] = {3.14f, 4.55f, 56.123f};
    
    file.write((char*)&n, 1);
    file.write((char*)buff, n);

}

std::unique_ptr<DataReader> Factory(const std::string& filename)
{
	std::string extension = filename.substr(filename.find_last_of('.') + 1);

	if (extension == "txt")
		return std::make_unique<TxtReader>(filename);
	else if (extension == "binf")
		return std::make_unique<BinFReader>(filename);
	return nullptr;
}

int main()
{
	uint8_t n;
	float buf[100];

    fill_file("input.binf");

	auto Reader = Factory("input.binf");
	if (Reader == nullptr)
		return -1;
	Reader->Open();
	Reader->Read();
	Reader->GetData((uint8_t*)buf, n);

	
	for (int i = 0; i < n / 4; i++)
		std::cout << buf[i] << std::endl;

	
	/*std::ifstream in("input2.bin", std::ios::binary);
	uint8_t n;
	in.read((char*)&n, 1);

	uint8_t* buf = new uint8_t[n];
	in.read((char*)buf, n);

	std::cout << (int)n << std::endl;

	for (int i = 0; i < n; i++)
		std::cout << (int)buf[i] << std::endl;

	delete[] buf;*/

	/*//Создание бинарного файла
	std::ofstream out("input2.bin", std::ios::binary);
	uint8_t buf[6];
	buf[0] = 5;
	for (int i = 0; i < 5; i++)
	{
		buf[i+1] = i+127;
	}

	out.write((char*)buf, 6);*/
}
