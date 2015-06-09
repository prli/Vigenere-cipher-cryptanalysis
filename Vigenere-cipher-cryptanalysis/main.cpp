#include<iostream>
#include<fstream>
#include <algorithm>
#include <string>

using namespace std;

string m_cipherText;
string m_plainText;
int m_keyLength;
string m_key;
char alphabet[26] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
//------------------{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10','11','12','13','14','15','16','17','18','19','20','21','22','23','24','25'};

string inputFile = "input.txt";
string outputFile = "output.txt";
string reportFile = "report.txt";

string readInputFile()
{
	string text;
	ifstream readfile;
	readfile.open(inputFile);
	if (readfile.is_open())
	{
		string line;
		while (getline(readfile, line))
		{
			std::transform(line.begin(), line.end(), line.begin(), ::tolower);
			line.erase(remove_if(line.begin(), line.end(), [](char c) { return !isalpha(c); }), line.end());
			text = text + line;
		}

		readfile.close();
	}
	return text;
}

void writeOutputFile()
{
	//clears file
	std::ofstream ofs;
	ofs.open(outputFile, ios::trunc);

	ofs << "Ciphered Text" << "\n";
	ofs << "============================================\n";
	ofs << m_cipherText << "\n";
	ofs << "============================================\n\n";

	ofs << "Key" << "\n";
	ofs << "============================================\n";
	ofs << m_key << "\n";
	ofs << "============================================\n\n";

	ofs << "Plain Text" << "\n";
	ofs << "============================================\n";
	ofs << m_plainText << "\n";
	ofs << "============================================\n\n";

	cout << "see " << outputFile << " for cipher and plain text\n";
	ofs.close();
}

void writeStatsFile(int* freq, int keyLength, string cipher, bool clearFile)
{
	std::ofstream ofs;
	ofs.open(reportFile, ios::app);

	ofs << "Key Length: " << keyLength << "\n";
	for (int i = 0; i < 26; i++)
	{
		double totalChar = (cipher.length() / keyLength);
		ofs << alphabet[i] << ": " << (double)*(freq + i) / totalChar * 100 << "%\t";
	}

	ofs << "\n-------------------------------\n";
	ofs.close();
}

void clearStatsFile()
{
	std::ofstream ofs;
	ofs.open(reportFile, ios::trunc);
	ofs.clear();
}

int* getFrequency(string cipher, int keyLength, int *frequency, int startPos)
{
	for (int i = startPos; i < (int)cipher.length(); i = i + keyLength)
	{
		int index = cipher[i] - 'a';
		frequency[index]++;
	}

	return frequency;
}

int getKeyLength(string cipher)
{
	clearStatsFile();
	for (int keyLength = 1; keyLength <= 100; keyLength++)
	{
		int frequency[26] = { 0 };
		getFrequency(cipher, keyLength, frequency, 0);
		writeStatsFile(frequency, keyLength, cipher, false);
		double totalChar = cipher.length() / keyLength;
		for (int i = 0; i < 26; i++)
		{
			if (((double)*(frequency + i) / totalChar * 100 >= 12) && (m_keyLength == 0))
			{
				m_keyLength = keyLength;
			}
		}

	}
	cout << "see " << reportFile << " for statistics for N key length\n";
	return m_keyLength;
}

string getKey(string cipher, int keyLength)
{
	string key;
	for (int j = 0; j < keyLength; j++)
	{
		int frequency[26] = { 0 };
		getFrequency(cipher, keyLength, frequency, j);
		for (int i = 0; i < 26; i++)
		{
			double totalChar = (cipher.length() / keyLength);
			double freqPerc = (double)frequency[i] / totalChar * 100;
			if (freqPerc >= 12)
			{
				key += alphabet[(i - 4 + 26) % 26]; //3 is e
			}
		}
	}

	return key;
}

char decryptedChar(char cipheredChar, char key)
{
	int keyPos = key - 'a';
	int cipherCharPos = cipheredChar - 'a';
	int plainCharPos = ((cipherCharPos - keyPos) + 26) % 26;
	char plainChar = alphabet[plainCharPos];
	return plainChar;
}

string decryptedText(string cipherText, string key)
{
	int textIndex = 0;
	int keyIndex = 0;
	string plainText = cipherText;

	while (textIndex < (int)cipherText.length())
	{
		plainText[textIndex] = decryptedChar(cipherText[textIndex], key[keyIndex]);
		textIndex++;
		keyIndex = (keyIndex + 1) % key.length();
	}

	return plainText;
}

void decrypt()
{
	cout << "Entering decrytion mode\n";
	cout << "copy cipher text into " << inputFile << "\n";
	system("pause");

	m_cipherText = readInputFile();
	if (m_cipherText.length() == 0)
	{
		cout << "No input text!\n";
		return;
	}

	m_keyLength = getKeyLength(m_cipherText);
	if (m_keyLength == 0)
	{
		cout << "Unable to determine key length, therefore unable to decrypt";
		system("pause");
		return;
	}

	m_key = getKey(m_cipherText, m_keyLength);
	if (m_key == "")
	{
		cout << "Unable to guess key, therefore unable to decrypt";
		system("pause");
		return;
	}

	m_plainText = decryptedText(m_cipherText, m_key);
	writeOutputFile();
}
string encrypt(string plainText, string key)
{
	string cipherText;
	for (int i = 0; i < (int)plainText.length(); i++)
	{
		int cipherPos = ((plainText[i] - 'a') + (key[i % key.length()] - 'a')) % 26;
		cipherText += alphabet[cipherPos];
	}
	return cipherText;
}
void encrypt()
{
	cout << "Entering encrytion mode\n";
	cout << "copy plain text into " << inputFile << "\n";
	system("pause");

	m_plainText = readInputFile();
	if (m_plainText.length() == 0)
	{
		cout << "No input text!\n";
		return;
	}
	cout << "enter key: ";
	cin >> m_key;
	m_keyLength = m_key.length();

	m_cipherText = encrypt(m_plainText, m_key);

	writeOutputFile();
}

void main()
{
	char mode;
	do
	{
		cout << "\nPlease select option to encrypt(e) or decrypt(d) or quit(q): ";
		cin >> mode;
		if (mode == 'e')
		{
			encrypt();
		}
		else if (mode == 'd')
		{
			decrypt();
		}
		else if (mode == 'q')
		{
			return;
		}
		else
		{
			cout << "Invalid selection! ";
		}
	} while (mode != 'e' || mode != 'd');

	system("pause");
}