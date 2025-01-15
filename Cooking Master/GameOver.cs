using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class GameOver : MonoBehaviour
{
    public Text GameOverText;
    // Start is called before the first frame update
    void Start()
    {
        string scores = "";
        for (int i = 0; i < GameManager.instance.highScores.Count; i++)
        {
            scores += $"{i + 1}: {GameManager.instance.highScores[i]} \n";
        }
        GameOverText.text = scores;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void StartNewRound()
    {
        SceneManager.LoadScene(1);
    }
}
