using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

/// <summary>
/// Main game script to hold all important game variables
/// </summary>
public class GameManager : MonoBehaviour
{
    public const string HighScoreFile = @"HighScores.txt";
    public bool activeGame = true;

    [Header("Player Vars")]
    public GameObject player1;
    public GameObject player2;

    public static List<Customer> customers = new List<Customer>();

    public static GameManager instance; //static instance of the game manager
    [Header("UI Vars")] //all the game UI variables
    public Text p1TimeUI;
    public Text p1ScoreUI;
    public Text p2TimeUI;
    public Text p2ScoreUI;

    //Values for player score and player time
    private float p1TimeValue = 120;
    private float p2TimeValue = 120;
    private int p1ScoreValue = 0;
    private int p2ScoreValue = 0;

    [Header("Vegetable Vars")]  //all the vegetable vars
    public Sprite lettuceSprite;
    public Sprite tomatoSprite;
    public Sprite onionSprite;

    [Header("Booster Variables")]
    public GameObject speedBoost;
    public GameObject timeBoost;
    public GameObject scoreBoost;

    public List<int> highScores = new List<int>();

    //public getters and setters for time and scores
    public float P1Time { get { return p1TimeValue; } set { p1TimeValue = value; } }
    public float P2Time { get { return p2TimeValue; } set { p2TimeValue = value; } }
    public int P1Score { get { return p1ScoreValue; } set { p1ScoreValue = value; } }
    public int P2Score { get { return p2ScoreValue; } set { p2ScoreValue = value; } }

    // Awake is called once script gets activated
    private void Awake()
    {
        if(instance == null)    //checks if instance is null
        {
            instance = this;    //set the instance to this
            DontDestroyOnLoad(this);
        }else if(instance != null && instance != this)  //if the instance isn't null and not equal to this
        {
            Destroy(this);  //destroy this so there isn't 2 game managers
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        //checks if highscore file exists and creates it if it doesn't
        if(File.Exists(HighScoreFile))
        {
            var fileContent = File.ReadAllText(HighScoreFile);
            string[] scores = fileContent.Split(',');
            for (int i = 0; i < scores.Length; i++)
            {
                if(scores[i] != "")
                    highScores.Add(System.Int32.Parse(scores[i]));
            }
        }else
        {
            File.Create(HighScoreFile);
        }
    }

    // Update is called once per frame
    void Update()
    {
        if(activeGame)
            UpdateUI(); //updates the time UI and score UI


        //makes the time value decrease by 1 seconds every second
        if (p1TimeValue <= 0)
        {
            player1.SetActive(false);
        }else
        {
            p1TimeValue -= Time.deltaTime;
        }


        //makes the time value decrease by 1 seconds every second
        if (p2TimeValue <= 0)
        {
            player2.SetActive(false);
        }else
        {
            p2TimeValue -= Time.deltaTime;
        }
        if(p1TimeValue <= 0 && p2TimeValue <= 0)
        {
            //gameover
            GameOver();
        }
        if(activeGame && !CustomersLeft())
        {
            //gameover
            GameOver();
        }
    }

    /// <summary>
    /// checks if any customers are left
    /// </summary>
    /// <returns>ture if customers left, false if no customer left</returns>
    bool CustomersLeft()
    {
        for (int i = 0; i < customers.Count; i++)
        {
            if (customers[i].gameObject.activeSelf == true)
                return true;
        }
        return false;
    }

    /// <summary>
    /// saves high scores and saves it to a file
    /// </summary>
    void GameOver()
    {
        int score = p1ScoreValue >= p2ScoreValue ? p1ScoreValue : p2ScoreValue;
        bool added = false;
        if(highScores.Count == 0 || highScores.Count < 10)
        {
            highScores.Add(score);
            highScores.Sort();
            added = true;
        }
        if (!added)
        {
            for (int i = 0; i < highScores.Count; i++)
            {
                if (score > highScores[i])
                {
                    highScores[i] = score;
                }
            }
        }
        SaveHighScores();
        activeGame = false;
        SceneManager.LoadScene(2);
    }

    /// <summary>
    /// saves high scores to a file
    /// </summary>
    void SaveHighScores()
    {
        using(StreamWriter sw = new StreamWriter(HighScoreFile))
        {
            for (int i = 0; i < highScores.Count; i++)
            {
                sw.Write(highScores[i]);
                if(i + 1 < highScores.Count)
                {
                    sw.Write(",");
                } 
            }
        }
    }

    /// <summary>
    /// Gets the vegetable sprite based on the vegetable passed
    /// </summary>
    /// <param name="_veg">What vegetable needs to be found</param>
    /// <returns></returns>
    public Sprite GetSprite(Vegetables _veg)
    {
        Sprite _sprite = null;  //sets the sprite to null in case None is passed
        switch (_veg)   //switch statement based on vegetable variable
        {
            case Vegetables.None:
                break;
            case Vegetables.Lettuce:
                _sprite = lettuceSprite;
                break;
            case Vegetables.Tomato:
                _sprite = tomatoSprite;
                break;
            case Vegetables.Onion:
                _sprite = onionSprite;
                break;
        }

        return _sprite;
    }

    /// <summary>
    /// Updates the UI score & time based
    /// </summary>
    void UpdateUI()
    {
        p1TimeUI.text = $"Time {(int)p1TimeValue}";
        p2TimeUI.text = $"Time {(int)p2TimeValue}";

        p1ScoreUI.text = $"Score {p1ScoreValue}";
        p2ScoreUI.text = $"Score {p2ScoreValue}";
    }

    /// <summary>
    /// Adds time to a player
    /// </summary>
    /// <param name="isPlayer1">Is this player 1?</param>
    /// <param name="timeToAdd">Time to add</param>
    public static void AddTime(bool isPlayer1, float timeToAdd)
    {
        if (isPlayer1)
        {
            instance.P1Time = instance.P1Time + timeToAdd;
        }else
        {
            instance.P2Time = instance.P2Time + timeToAdd;
        }
    }

    /// <summary>
    /// Add score to a player
    /// </summary>
    /// <param name="isPlayer1">Is this player 1?</param>
    /// <param name="scoreToAdd">Score to add</param>
    public static void AddScore(bool isPlayer1, int scoreToAdd)
    {
        if (isPlayer1)
        {
            instance.P1Score = instance.P1Score + scoreToAdd;
        }
        else
        {
            instance.P2Score = instance.P2Score + scoreToAdd;
        }
    }

    /// <summary>
    /// Drops a random buff
    /// </summary>
    /// <param name="player">Takes a player to set layer who can pick it up</param>
    public void DropBuff(Player player)
    {
        int index = Random.Range(0, 3);
        GameObject prefab = null;
        switch (index)
        {
            case 0:
                prefab = speedBoost;
                break;
            case 1:
                prefab = timeBoost;
                break;
            case 2:
                prefab = scoreBoost;
                break;
        }
        Vector3 ranPos = new Vector3(
                Random.Range(-5.5f, 6.5f),
                Random.Range(-3.5f, 1.5f),
                -1
            );
        GameObject go = Instantiate(prefab, ranPos, Quaternion.identity);
        go.layer = player.gameObject.layer;
    }
}
