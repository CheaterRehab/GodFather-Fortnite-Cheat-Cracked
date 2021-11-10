#include "stdafx.h"
Driver::Module a;
void runcheat()
{
	while (a.addr == 0)
	{
		a = driver->GetModuleBase(L"FortniteClient-Win64-Shipping.exe");
	}
	auto uworld = read<DWORD_PTR>(a.addr + 0x9df4ce0);
}

int main() {
	printf("[!] Open driver connection: ");
	if (driver->Init(FALSE)) {
		printf("Success!\n");
		driver->Attach(L"FortniteClient-Win64-Shipping.exe");
		while (true)
		{
			clock_t begin = clock();
			runcheat();

		}
		return 0;
	}
	printf("Failed!\n");
	system("pause");
	return 1;
}