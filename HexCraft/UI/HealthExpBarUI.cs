using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class HealthExpBarUI : MonoBehaviour
{
    public Image healthBar;
    public Image expBar;
    public TextMeshProUGUI levelText;

    private void Update()
    {
    }

    public void UpdateUI()
    {
        healthBar.fillAmount = GameManager.playerInstance.GetHealthPercent;
        expBar.fillAmount = GameManager.playerInstance.GetEXPPercent;
        levelText.SetText(GameManager.playerInstance.Level.ToString());
    }
}
