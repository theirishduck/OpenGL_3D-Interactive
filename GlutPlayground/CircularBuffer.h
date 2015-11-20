#pragma once

template <typename T>
class CircularBuffer
{
public:
	CircularBuffer<T>(int size)
	{
		m_size = size;
		m_cnt = 0;
		m_cursor = size - 1;
		m_buffer = new T[size];
	}

	~CircularBuffer<T>()
	{
		delete[] m_buffer;
	}

	void push(T v)
	{
		++m_cursor;
		m_cursor %= m_size;
		m_buffer[m_cursor] = v;

		if (m_cnt < m_size)
			m_cnt++;
	}

	int getCursor() const
	{
		return m_cursor;
	}

	T diff()
	{
		int dest = m_cursor - (m_size - 1);
		dest %= m_size;
		if (dest < 0)
			dest += m_size;

		return (m_buffer[m_cursor] - m_buffer[dest]);
	}

	bool isFull()
	{
		return m_cnt >= m_size;
	}

private:
	int m_size;
	int m_cnt;
	int m_cursor;

	T *m_buffer;

};

