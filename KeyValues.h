#pragma once

#include <cstdio>
#include <cstring>

class KeyValues
{
public:
	static KeyValues* Parse( const char* buffer, size_t size );

	void SetName( const char* name ) { snprintf( m_caName, 250, name ); }

	void SetInt( int value ) { m_iValue = value; }
	void SetFloat( float value ) { m_fValue = value; }
	void SetString( const char* value ) { char* newname = new char[strlen( value )+1]; strcpy( newname, value ); m_pkcValue = newname; }

	const char* GetName() { return m_caName; }

	int GetInt() { return m_iValue; }
	float GetFloat() { return m_fValue; }
	const char* GetString() { return m_pkcValue; }

	void SetPreviousKey( KeyValues* pKev ) { m_pPrev = pKev; }
	void SetSuperKey( KeyValues* pKey ) { m_pSuper = pKey; }
	void SetSubKey( KeyValues* pKey ) { m_pSub = pKey; }
	void SetNextKey( KeyValues* pKev ) { m_pNext = pKev; }

	KeyValues* GetPreviousKey() { return m_pPrev; }
	KeyValues* GetSuperKey() { return m_pSuper; }
	KeyValues* GetSubKey() { return m_pSub; }
	KeyValues* GetNextKey() { return m_pNext; }

private:
	char m_caName[250];
	
	union
	{
		int m_iValue;
		float m_fValue;
		const char* m_pkcValue;
	};

	KeyValues* m_pPrev = NULL;
	KeyValues* m_pSuper = NULL;
	KeyValues* m_pSub = NULL;
	KeyValues* m_pNext = NULL;
};

KeyValues* KeyValues::Parse( const char* buffer, size_t size )
{
	int pos = 0;
	char string[250];

	KeyValues* pKeyValues = new KeyValues;
	KeyValues* pCurrentKeyValues = pKeyValues;

	bool readingstring = false;
	int stringstartpos;
	bool nameread = false, activechain = true;
	while ( pos < size )
	{
		if ( buffer[pos] == '\"' )
		{
			if ( readingstring == true )
			{
				memcpy( string, &buffer[stringstartpos], pos - stringstartpos );
				string[pos - stringstartpos] = NULL;

				if ( nameread )
				{
					pCurrentKeyValues->SetString( string );
					nameread = false;
				}
				else
				{
					pCurrentKeyValues->SetName( string );
					nameread = true;
				}
				readingstring = false;
			}
			else
			{
				stringstartpos = pos + 1;
				readingstring = true;
				if ( !nameread && !activechain )
				{
					pCurrentKeyValues->SetNextKey( new KeyValues() );
					pCurrentKeyValues->GetNextKey()->SetPreviousKey( pCurrentKeyValues );
					pCurrentKeyValues->GetNextKey()->SetSuperKey( pCurrentKeyValues->GetSuperKey() );
					pCurrentKeyValues = pCurrentKeyValues->GetNextKey();
				}
				activechain = false;
			}
		}
		else if ( buffer[pos] == '\n' )
		{
			readingstring = false;
			nameread = false;
		}
		else if ( buffer[pos] == '{' )
		{
			activechain = true;
			pCurrentKeyValues->SetSubKey( new KeyValues() );
			pCurrentKeyValues->GetSubKey()->SetSuperKey( pCurrentKeyValues );
			pCurrentKeyValues = pCurrentKeyValues->GetSubKey();
		}
		else if ( buffer[pos] == '}' )
		{
			if ( pCurrentKeyValues->GetSuperKey() )
			{
				pCurrentKeyValues = pCurrentKeyValues->GetSuperKey();
			}
			else
			{
				break;
			}
		}
		pos++;
	}

	return pKeyValues;
}