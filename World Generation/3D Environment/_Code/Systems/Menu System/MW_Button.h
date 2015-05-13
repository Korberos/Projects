#include "./Menu Widget.h"

class MW_Button : public MenuWidget
{
private:
	int				m_nX;
	int				m_nY;
	int				m_nTexture;

	enum eStates	{	STATE_NONE = 0, STATE_MOUSEOVER, STATE_PUSHDOWN, STATE_COUNT };
	unsigned int	m_nState;

	float*			m_fTime;
	float			m_fTimeDown;

public:
	MW_Button();
	~MW_Button();

			bool	Initialize(void);
			int		Input(void);
			int		Update(void);
			void	Render(void);

	inline	void	ButtonDown(void)	{}

	inline	void	SetX(int X)					{ m_nX = X;					}
	inline	void	SetY(int Y)					{ m_nY = Y;					}
	inline	void	SetTexture(int nTexture)	{ m_nTexture = nTexture;	}
	inline	void	LinkToTime(float* fTime)	{ m_fTime = fTime;			}
};